// The browser REPL: wires an Io VM to the page's #output, #input, #eval-btn,
// #clear-btn, #status, #repl and .topic elements. Used by browser/index.html
// and the site's /repl/ page.

const AWAITING_DELAY_MS = 50; // show "awaiting..." only for evals that actually wait

export async function startRepl(loadVm, doc = document) {
	const outputEl = doc.getElementById("output");
	const inputEl = doc.getElementById("input");
	const statusEl = doc.getElementById("status");
	const replEl = doc.getElementById("repl");
	if (!outputEl || !inputEl || !statusEl || !replEl) return null;

	const history = [];
	let historyIndex = -1;
	let vm = null;

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
		// Evals that await a JS Promise show a placeholder until they finish.
		const timer = setTimeout(() => {
			resultLine = appendLine("result", "⏳ awaiting...", pair);
		}, AWAITING_DELAY_MS);

		let status, output;
		try {
			({ status, output } = await vm.eval(code));
		} catch (e) {
			status = -1;
			output = "Error: " + e.message;
		}
		clearTimeout(timer);

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
		} else if (e.key === "u" && e.ctrlKey && e.shiftKey) {
			e.preventDefault();
			clearOutput();
		}
	});

	doc.getElementById("eval-btn")?.addEventListener("click", handleEval);
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
