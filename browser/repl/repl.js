// The browser REPL: wires an Io VM to the page's #output, #input, #eval-btn,
// #clear-btn, #status, #repl and .topic elements. Used by browser/index.html
// and the site's /repl/ page.
//
// While an eval is running, #eval-btn reads "Stop" and interrupts it, as does
// Ctrl-C in the input. Code that never waits can only be stopped if the VM
// was loaded with a timeSlice.

const AWAITING_DELAY_MS = 50; // show "running..." only for evals that take a while

export async function startRepl(loadVm, doc = document) {
	const outputEl = doc.getElementById("output");
	const inputEl = doc.getElementById("input");
	const statusEl = doc.getElementById("status");
	const replEl = doc.getElementById("repl");
	if (!outputEl || !inputEl || !statusEl || !replEl) return null;

	const history = [];
	let historyIndex = -1;
	let vm = null;
	let running = 0;   // evals started and not yet finished
	const evalBtn = doc.getElementById("eval-btn");
	const evalLabel = evalBtn ? evalBtn.textContent : "";
	const stopLabel = evalLabel === evalLabel.toUpperCase() ? "STOP" : "Stop";

	function showStop(on) {
		if (evalBtn) evalBtn.textContent = on ? stopLabel : evalLabel;
	}

	function scrollToEnd() {
		outputEl.scrollTop = outputEl.scrollHeight;
	}

	function appendLine(className, text, pair = doc.createElement("div")) {
		pair.className = "replPair";
		const line = doc.createElement("div");
		line.className = className;
		line.textContent = text;
		pair.appendChild(line);
		outputEl.appendChild(pair);
		scrollToEnd();
		return line;
	}

	function autoResize() {
		inputEl.style.height = "auto";
		inputEl.style.height = inputEl.scrollHeight + "px";
	}

	async function handleEval() {
		const code = inputEl.value.trim();
		if (!code || !vm) return;

		history.push(code);
		historyIndex = history.length;
		inputEl.value = "";
		autoResize();

		const pair = doc.createElement("div");
		appendLine("input-echo", code, pair);
		let resultLine = null;
		// Evals that take a while show a placeholder until they finish.
		const timer = setTimeout(() => {
			resultLine = appendLine("result", "⏳ running...", pair);
			showStop(true);
		}, AWAITING_DELAY_MS);
		running++;

		let status, output;
		try {
			({ status, output } = await vm.eval(code));
		} catch (e) {
			status = -1;
			output = "Error: " + e.message;
		}
		clearTimeout(timer);
		if (--running === 0) showStop(false);

		if (resultLine) {
			resultLine.textContent = output || "(done)";
			resultLine.className = status !== 0 ? "result-error" : "result";
			scrollToEnd();
		} else if (output) {
			appendLine(status !== 0 ? "result-error" : "result", output, pair);
		}
	}

	function clearOutput() {
		outputEl.innerHTML = "";
	}

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
		} else if (e.key === "c" && e.ctrlKey && running > 0 && inputEl.selectionStart === inputEl.selectionEnd) {
			e.preventDefault();
			vm.interrupt();
		} else if (e.key === "u" && e.ctrlKey && e.shiftKey) {
			e.preventDefault();
			clearOutput();
		}
	});

	evalBtn?.addEventListener("click", () => {
		if (running > 0 && evalBtn.textContent === stopLabel) vm.interrupt();
		else handleEval();
	});
	doc.getElementById("clear-btn")?.addEventListener("click", clearOutput);

	doc.querySelectorAll(".topic").forEach((el) => {
		el.addEventListener("click", () => {
			inputEl.value = el.dataset.code;
			autoResize();
			handleEval();
		});
	});

	try {
		vm = await loadVm();
		// Output from callbacks run outside an eval (setTimeout, event handlers)
		vm.onOutput((text) => appendLine("result", text));
		statusEl.className = "status hidden";
		replEl.style.opacity = "1";
		inputEl.disabled = false;
		inputEl.placeholder = "";
		inputEl.focus();
	} catch (e) {
		statusEl.textContent = "Failed to load: " + e.message;
		statusEl.className = "status error";
		console.error(e);
	}
	return vm;
}
