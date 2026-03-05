// Io VM — Browser WASM loader + WASI shim + REPL logic

const IO_WASM_URL = "io_browser.wasm";

let wasm = null;    // WebAssembly instance
let memory = null;  // WASM linear memory

// --- Minimal WASI shim ---
// The reactor binary still imports WASI symbols.
// We only need to support what the VM actually calls.

function getMemoryView() {
	return new DataView(memory.buffer);
}

function readCString(ptr) {
	const bytes = new Uint8Array(memory.buffer);
	let end = ptr;
	while (bytes[end] !== 0) end++;
	return new TextDecoder().decode(bytes.subarray(ptr, end));
}

function writeCString(ptr, str) {
	const bytes = new Uint8Array(memory.buffer);
	const encoded = new TextEncoder().encode(str);
	bytes.set(encoded, ptr);
	bytes[ptr + encoded.length] = 0;
	return encoded.length;
}

// Captured stdout/stderr from fd_write (before io_init installs printCallback)
let earlyOutput = "";

const wasi_snapshot_preview1 = {
	// fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) -> errno
	fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) {
		const view = getMemoryView();
		const bytes = new Uint8Array(memory.buffer);
		let totalWritten = 0;

		for (let i = 0; i < iovs_len; i++) {
			const base = view.getUint32(iovs_ptr + i * 8, true);
			const len = view.getUint32(iovs_ptr + i * 8 + 4, true);
			const chunk = bytes.subarray(base, base + len);
			const text = new TextDecoder().decode(chunk);

			if (fd === 1 || fd === 2) {
				earlyOutput += text;
			}
			totalWritten += len;
		}

		view.setUint32(nwritten_ptr, totalWritten, true);
		return 0; // ESUCCESS
	},

	// fd_read(fd, iovs_ptr, iovs_len, nread_ptr) -> errno
	fd_read(fd, iovs_ptr, iovs_len, nread_ptr) {
		const view = getMemoryView();
		view.setUint32(nread_ptr, 0, true);
		return 0; // EOF
	},

	// fd_close(fd) -> errno
	fd_close(fd) { return 0; },

	// fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) -> errno
	fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) {
		const view = getMemoryView();
		view.setBigUint64(newoffset_ptr, 0n, true);
		return 0;
	},

	// fd_fdstat_get(fd, buf) -> errno
	fd_fdstat_get(fd, buf) {
		const view = getMemoryView();
		view.setUint8(buf, fd <= 2 ? 2 : 4); // CHARACTER_DEVICE for stdio
		view.setUint16(buf + 2, 0, true);     // flags
		view.setBigUint64(buf + 8, 0n, true); // rights_base
		view.setBigUint64(buf + 16, 0n, true); // rights_inheriting
		return 0;
	},

	// fd_fdstat_set_flags(fd, flags) -> errno
	fd_fdstat_set_flags(fd, flags) { return 0; },

	// fd_filestat_set_size(fd, size) -> errno
	fd_filestat_set_size(fd, size) { return 70; }, // ENOSYS

	// fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) -> errno
	fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) {
		const view = getMemoryView();
		view.setUint32(bufused_ptr, 0, true);
		return 0;
	},

	// fd_renumber(fd, to) -> errno
	fd_renumber(fd, to) { return 70; }, // ENOSYS

	// fd_prestat_get(fd, buf) -> errno
	fd_prestat_get(fd, buf) { return 8; }, // EBADF — no preopened dirs

	// fd_prestat_dir_name(fd, path, path_len) -> errno
	fd_prestat_dir_name(fd, path, path_len) { return 8; }, // EBADF

	// path_open — not supported in browser
	path_open() { return 76; }, // ENOTCAPABLE

	// path_create_directory(fd, path, path_len) -> errno
	path_create_directory(fd, path, path_len) { return 76; },

	// path_filestat_get(fd, flags, path, path_len, buf) -> errno
	path_filestat_get(fd, flags, path, path_len, buf) { return 76; },

	// path_remove_directory(fd, path, path_len) -> errno
	path_remove_directory(fd, path, path_len) { return 76; },

	// path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) -> errno
	path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) { return 76; },

	// path_unlink_file(fd, path, path_len) -> errno
	path_unlink_file(fd, path, path_len) { return 76; },

	// clock_time_get(clock_id, precision, time_ptr) -> errno
	clock_time_get(clock_id, precision, time_ptr) {
		const view = getMemoryView();
		const now = BigInt(Math.floor(performance.now() * 1_000_000)); // ns
		view.setBigUint64(time_ptr, now, true);
		return 0;
	},

	// environ_sizes_get(count_ptr, buf_size_ptr) -> errno
	environ_sizes_get(count_ptr, buf_size_ptr) {
		const view = getMemoryView();
		view.setUint32(count_ptr, 0, true);
		view.setUint32(buf_size_ptr, 0, true);
		return 0;
	},

	// environ_get(environ_ptr, buf_ptr) -> errno
	environ_get(environ_ptr, buf_ptr) { return 0; },

	// args_sizes_get(argc_ptr, buf_size_ptr) -> errno
	args_sizes_get(argc_ptr, buf_size_ptr) {
		const view = getMemoryView();
		view.setUint32(argc_ptr, 0, true);
		view.setUint32(buf_size_ptr, 0, true);
		return 0;
	},

	// args_get(argv_ptr, buf_ptr) -> errno
	args_get(argv_ptr, buf_ptr) { return 0; },

	// proc_exit(code)
	proc_exit(code) {
		throw new Error(`Io called exit(${code})`);
	},

	// random_get(buf, buf_len) -> errno
	random_get(buf, buf_len) {
		const bytes = new Uint8Array(memory.buffer, buf, buf_len);
		crypto.getRandomValues(bytes);
		return 0;
	},

	// sched_yield() -> errno
	sched_yield() { return 0; },

	// poll_oneoff — stub
	poll_oneoff(in_ptr, out_ptr, nsubscriptions, nevents_ptr) {
		const view = getMemoryView();
		view.setUint32(nevents_ptr, 0, true);
		return 0;
	},
};

// --- WASM loader ---

async function loadIo() {
	const importObject = { wasi_snapshot_preview1 };

	const response = await fetch(IO_WASM_URL);
	const { instance } = await WebAssembly.instantiateStreaming(response, importObject);

	wasm = instance;
	memory = wasm.exports.memory;

	// Reactor model: call _initialize to run static constructors
	if (wasm.exports._initialize) {
		wasm.exports._initialize();
	}

	// Initialize the Io VM
	wasm.exports.io_init();

	return wasm;
}

// --- REPL interface ---

function ioEval(code) {
	if (!wasm) throw new Error("Io VM not loaded");

	// Write code into the C-side input buffer
	const inputBufPtr = wasm.exports.io_get_input_buf();
	const inputBufSize = wasm.exports.io_get_input_buf_size();
	const encoded = new TextEncoder().encode(code);

	if (encoded.length >= inputBufSize) {
		return { status: -1, output: "Error: input too long (max " + (inputBufSize - 1) + " bytes)" };
	}

	const bytes = new Uint8Array(memory.buffer);
	bytes.set(encoded, inputBufPtr);
	bytes[inputBufPtr + encoded.length] = 0; // null-terminate

	earlyOutput = "";
	const status = wasm.exports.io_eval_input();

	// Read output
	const outPtr = wasm.exports.io_get_output();
	const outLen = wasm.exports.io_get_output_len();
	let output = "";

	if (outLen > 0) {
		output = readCString(outPtr);
	}

	// Include any early fd_write output (from before printCallback was set)
	if (earlyOutput) {
		output = earlyOutput + output;
		earlyOutput = "";
	}

	return { status, output };
}

// --- UI wiring ---

let outputEl, inputEl;
const history = [];
let historyIndex = -1;

function appendReplPair(inputText, outputText, isError) {
	const pair = document.createElement("div");
	pair.className = "replPair";

	const inputLine = document.createElement("div");
	inputLine.className = "input-echo";
	inputLine.textContent = inputText;
	pair.appendChild(inputLine);

	if (outputText) {
		const resultLine = document.createElement("div");
		resultLine.className = isError ? "result-error" : "result";
		resultLine.textContent = outputText;
		pair.appendChild(resultLine);
	}

	outputEl.appendChild(pair);
	outputEl.scrollTop = outputEl.scrollHeight;
}

function autoResize() {
	inputEl.style.height = "auto";
	inputEl.style.height = inputEl.scrollHeight + "px";
}

function handleEval() {
	const code = inputEl.value.trim();
	if (!code) return;

	history.push(code);
	historyIndex = history.length;

	inputEl.value = "";
	autoResize();

	try {
		const { status, output } = ioEval(code);
		appendReplPair(code, output || null, status !== 0);
	} catch (e) {
		appendReplPair(code, "Error: " + e.message, true);
	}
}

function clearOutput() {
	outputEl.innerHTML = "";
}

function initUI() {
	outputEl = document.getElementById("output");
	inputEl = document.getElementById("input");

	inputEl.addEventListener("input", autoResize);

	inputEl.addEventListener("keydown", (e) => {
		if (e.key === "Enter" && (e.ctrlKey || e.metaKey)) {
			e.preventDefault();
			handleEval();
		} else if (e.key === "Enter" && !e.shiftKey && inputEl.value.indexOf("\n") === -1) {
			// Single-line: Enter evals. Multi-line (shift+enter): newline.
			e.preventDefault();
			handleEval();
		} else if (e.key === "ArrowUp" && inputEl.selectionStart === 0) {
			if (historyIndex > 0) {
				historyIndex--;
				inputEl.value = history[historyIndex];
				autoResize();
			}
			e.preventDefault();
		} else if (e.key === "ArrowDown" && inputEl.selectionStart === inputEl.value.length) {
			if (historyIndex < history.length - 1) {
				historyIndex++;
				inputEl.value = history[historyIndex];
			} else {
				historyIndex = history.length;
				inputEl.value = "";
			}
			autoResize();
			e.preventDefault();
		} else if (e.key === "u" && e.ctrlKey && e.shiftKey) {
			e.preventDefault();
			clearOutput();
		}
	});

	document.getElementById("eval-btn").addEventListener("click", handleEval);
	document.getElementById("clear-btn").addEventListener("click", clearOutput);
}

// --- Boot ---

async function boot() {
	const statusEl = document.getElementById("status");
	const replEl = document.getElementById("repl");

	try {
		await loadIo();
		statusEl.textContent = "Ready";
		statusEl.className = "status ready";
		replEl.style.opacity = "1";
		inputEl.disabled = false;
		inputEl.placeholder = "";
		inputEl.focus();
	} catch (e) {
		statusEl.textContent = "Failed to load: " + e.message;
		statusEl.className = "status error";
		console.error(e);
	}
}

document.addEventListener("DOMContentLoaded", () => {
	initUI();
	boot();
});
