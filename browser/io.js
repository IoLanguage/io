// Classic-script shim over the Io module (src/index.js) and the REPL
// (repl/repl.js), for pages that load <script src="io.js">. It keeps the old
// globals working — loadIo(), ioEval(), io, pendingAsyncResolve — and boots
// the REPL when the page has its elements. New code should import the module:
//
//   import { Io } from "./src/index.js";
//
// Pages outside browser/ can point at the wasm by setting window.IO_WASM_URL
// (relative to the page) before loading this script.

const ioScriptUrl = document.currentScript ? document.currentScript.src : location.href;
const ioModules = Promise.all([
	import(new URL("src/index.js", ioScriptUrl)),
	import(new URL("repl/repl.js", ioScriptUrl)),
]);
let ioVm = null;

// Set by callers after ioEval returns status 2 (yielded on a JS Promise);
// called with { status, output } when the eval finishes.
let pendingAsyncResolve = null;

async function loadIo() {
	if (ioVm) return ioVm;
	const [{ Io }] = await ioModules;
	const wasm = window.IO_WASM_URL || new URL("io_browser.wasm", ioScriptUrl);
	ioVm = await Io.load({ wasm });
	return ioVm;
}

function ioEval(code) {
	if (!ioVm) throw new Error("Io VM not loaded");
	const result = ioVm.evalNow(code);
	if (result.status === 2) {
		ioVm.awaiting = (final) => {
			const resolve = pendingAsyncResolve;
			pendingAsyncResolve = null;
			if (resolve) resolve(final);
		};
	}
	return result;
}

const io = {
	get lobby() {
		if (!ioVm) throw new Error("Io VM not loaded");
		return ioVm.lobby;
	},

	send(handle, msg, ...args) {
		if (!ioVm) throw new Error("Io VM not loaded");
		return ioVm.send(handle, msg, ...args);
	},
};

document.addEventListener("DOMContentLoaded", async () => {
	const [, { startRepl }] = await ioModules;
	startRepl(loadIo);
});
