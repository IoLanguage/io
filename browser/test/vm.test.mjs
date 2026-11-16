// Node tests for the Io module — no browser needed.
// Usage: make check-browser-node   (or node --test browser/test/vm.test.mjs)

import { test } from "node:test";
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { Io } from "../src/index.js";

const io = await Io.load();

test("eval returns the result", async () => {
	assert.deepEqual(await io.eval("list(1, 2, 3) map(x, x * x) sum"), { status: 0, output: "==> 14\n" });
});

test("eval returns printed output", async () => {
	assert.deepEqual(await io.eval('"hi" println'), { status: 0, output: "hi\n" });
});

test("an Io exception gives status 1", async () => {
	const { status, output } = await io.eval("1 foo");
	assert.equal(status, 1);
	assert.match(output, /does not respond to 'foo'/);
});

test("state persists between evals", async () => {
	await io.eval("counter := 41");
	assert.equal((await io.eval("counter + 1")).output, "==> 42\n");
});

test("lobby slots can be set from JS", async () => {
	io.lobby.greeting = "hello from JS";
	assert.equal((await io.eval("greeting")).output, "==> hello from JS\n");
});

test("messages can be sent from JS", async () => {
	await io.eval("double := method(x, x * 2)");
	assert.equal(io.lobby.double(21), 42);
});

test("Io reaches JS globals", async () => {
	assert.equal((await io.eval("JS Math sqrt(16)")).output, "==> 4\n");
});

test("eval settles after awaiting a JS Promise", async () => {
	globalThis.later = (v) => new Promise((resolve) => setTimeout(() => resolve(v), 20));
	assert.deepEqual(await io.eval("JS later(42) await + 1"), { status: 0, output: "==> 43\n" });
	assert.equal((await io.eval('try(JS get("Promise") reject("boom") await) error containsSeq("boom")')).output, "==> true\n");
});

test("evals run in order while one awaits", async () => {
	const first = io.eval("JS later(1) await");
	const second = io.eval("2");
	assert.equal((await first).output, "==> 1\n");
	assert.equal((await second).output, "==> 2\n");
});

test("callback output goes to onOutput, once", async () => {
	await io.eval('"earlier" println');
	const seen = [];
	const off = io.onOutput((text) => seen.push(text));
	await io.eval('JS setTimeout(block("tick" println), 10)');
	await new Promise((resolve) => setTimeout(resolve, 50));
	off();
	assert.deepEqual(seen, ["tick\n"]);
});

test("VMs are independent", async () => {
	const other = await Io.load({ wasm: await readFile(new URL("../io_browser.wasm", import.meta.url)) });
	await other.eval("onlyHere := 1");
	assert.equal((await io.eval("onlyHere")).status, 1);
	assert.equal((await other.eval("onlyHere")).output, "==> 1\n");
	other.dispose();
});

test("dispose settles a pending await", async () => {
	const vm = await Io.load();
	const pending = vm.eval("JS later(1) await");
	await new Promise((resolve) => setTimeout(resolve, 0));
	vm.dispose();
	assert.deepEqual(await pending, { status: -1, output: "Io VM disposed" });
	await assert.rejects(vm.eval("1"), /not loaded/);
});
