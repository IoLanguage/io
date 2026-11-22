// Node tests for the Io module — no browser needed.
// Usage: make check-browser-node   (or node --test browser/test/vm.test.mjs)

import { test } from "node:test";
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import { Io } from "../src/index.js";

const io = await Io.load();
globalThis.later = (v) => new Promise((resolve) => setTimeout(() => resolve(v), 20));

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

test("a syntax error gives status 1 and the VM carries on", async () => {
	for (const src of ["x := (", "foo(1, ", '"abc', "a(b(c"]) {
		const { status, output } = await io.eval(src);
		assert.equal(status, 1, src);
		assert.match(output, /compile error/, src);
	}
	assert.equal((await io.eval("list(1, 2, 3) map(x, x * x) sum")).output, "==> 14\n");
});

test("an uncaught error prints the calls that led to it", async () => {
	const src = "btInner := method(r := 1 + nil; r)\nbtOuter := method(r := btInner; r)\nbtOuter";
	const { status, output } = await io.eval(src);
	assert.equal(status, 1);
	assert.match(output, /Number \+ .* 1\n\s+Object btInner .* 2\n\s+Object btOuter .* 3\n/);
	assert.doesNotMatch(output, /nothing on stack|hostEval/);
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

// --- Scheduler: Io coroutines inside the JS event loop ---

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

test("wait parks the eval without blocking JS", async () => {
	let ticks = 0;
	const interval = setInterval(() => ticks++, 10);
	const started = Date.now();
	assert.deepEqual(await io.eval('wait(0.15); "waited"'), { status: 0, output: "==> waited\n" });
	clearInterval(interval);
	assert.ok(Date.now() - started >= 140);
	assert.ok(ticks >= 5, `JS timers ran ${ticks} times during the wait`);
});

test("actor messages run and their futures resolve", async () => {
	await io.eval("Counter := Object clone do(n := 0; add := method(x, n = n + x; n))");
	assert.equal((await io.eval("f := Counter @add(5); f + 1")).output, "==> 6\n");
	assert.equal((await io.eval("Counter @add(1) asString size")).output, "==> 1\n");
});

test("an actor can wait and await while its caller waits on it", async () => {
	await io.eval("Slow := Object clone do(twice := method(n, wait(0.02); JS later(n) await * 2))");
	assert.equal((await io.eval("a := Slow @twice(5); b := Slow @twice(6); (a + 1) + (b + 1)")).output, "==> 24\n");
});

test("a background coroutine wakes on its timer with no eval running", async () => {
	const seen = [];
	const off = io.onOutput((text) => seen.push(text));
	assert.equal((await io.eval('coroDoLater(wait(0.03); "woke" println); "started"')).output, "==> started\n");
	await sleep(80);
	off();
	assert.deepEqual(seen, ["woke\n"]);
});

test("coroutines await different Promises at the same time", async () => {
	const result = await io.eval(
		"coroDoLater(Lobby slowOne := JS later(1) await); coroDoLater(Lobby fastOne := JS later(2) await); wait(0.06); list(slowOne, fastOne)");
	assert.equal(result.output, "==> list(1, 2)\n");
});

test("a yielding loop shares time with the eval that started it", async () => {
	const result = await io.eval(
		"Lobby spins := 0; Lobby stop := false; coroDoLater(while(stop not, spins = spins + 1; yield)); wait(0.05); stop = true; spins > 10");
	assert.equal(result.output, "==> true\n");
});

test("a message chain sent to a pending Promise applies once", async () => {
	globalThis.laterWords = () => new Promise((resolve) => setTimeout(() => resolve("hello big world"), 5));
	assert.equal((await io.eval('JS laterWords split(" ") size')).output, "==> 3\n");
});

test("an Io callback run by JS can wait", async () => {
	const seen = [];
	const off = io.onOutput((text) => seen.push(text));
	await io.eval('JS setTimeout(block(wait(0.03); "late" println), 5)');
	await sleep(80);
	off();
	assert.deepEqual(seen, ["late\n"]);
});

test("a JS call into Io that waits returns null and finishes later", async () => {
	await io.eval("Lobby slowSet := method(wait(0.03); Lobby slowDone := true; 9); Lobby slowDone := false");
	assert.equal(io.lobby.slowSet(), null);
	assert.equal((await io.eval("slowDone")).output, "==> false\n");
	await sleep(60);
	assert.equal((await io.eval("slowDone")).output, "==> true\n");
});

test("an error in a background coroutine goes to onOutput", async () => {
	const seen = [];
	const off = io.onOutput((text) => seen.push(text));
	await io.eval("coroDoLater(1 nope); nil");
	await sleep(20);
	off();
	assert.match(seen.join(""), /does not respond to 'nope'/);
});

test("waiting inside a synchronous JS callback is an error, not a hang", async () => {
	globalThis.mapThree = (f) => [1, 2, 3].map(f);
	assert.equal((await io.eval("JS mapThree(block(x, x * 2))")).output, "==> list(2, 4, 6)\n");
	const { status, output } = await io.eval("JS mapThree(block(x, wait(0.01); x))");
	assert.equal(status, 1);
	assert.match(output, /can't wait or switch coroutines here/);
	assert.equal((await io.eval('wait(0.01); "fine"')).output, "==> fine\n");
});

test("an Io closure handed to JS keeps its variables", async () => {
	globalThis.keep = (f) => { globalThis.kept = f; };
	await io.eval("Lobby makeAdder := method(x, block(y, x + y)); JS keep(makeAdder(5)); nil");
	// Method calls reuse recycled locals objects; the block's scope must not be one
	await io.eval("churn := method(a, b, a); 20 repeat(churn(1, 2)); nil");
	assert.equal(globalThis.kept(3), 8);
});

test("settled Promises give back their handles", async () => {
	// More Promises than the Io-side handle table starts with (1024 slots)
	for (let i = 0; i < 1300; i++) {
		const { output } = await io.eval('JS get("Promise") resolve(' + i + ") await");
		if (output !== `==> ${i}\n`) assert.fail(`await ${i} gave ${JSON.stringify(output)}`);
	}
	const before = io.handles.map.size;
	await io.eval("Collector collect");
	assert.ok(io.handles.map.size < before, "collected Futures release their JS Promise handles");
});

test("code run by evalArgAt, perform, do, lexicalDo, interpolate, init, repeat and Sequence foreach can wait", async () => {
	// Each of these used to run its code in a nested C eval loop, where a
	// coroutine switch raises "can't wait or switch coroutines here".
	const cases = [
		["list(1, 2, 3) map(x, wait(0.001); x * 2)", "list(2, 4, 6)"],
		['m := Map clone atPut("a", 1); m select(k, v, wait(0.001); true) size', "1"],
		["f := method(call evalArgAt(0)); f(wait(0.001); 3)", "3"],
		['B := Object clone do(m := method(a, wait(0.001); a)); B perform("m", 6)', "6"],
		['B performWithArgList("m", list(9))', "9"],
		["A := Object clone do(init := method(wait(0.001); self v := 7; 99)); A clone v", "7"],
		["o := Object clone; (o do(wait(0.001); x := 12)) x", "12"],
		["n := 0; Object clone do(a := 1) foreachSlot(k, v, wait(0.001); n = n + 1); n", "1"],
		["w := Object clone do(work := method(wait(0.001); 21)); list(1, 2) map(x, w @work + x)", "list(22, 23)"],
		["n := 0; 3 repeat(wait(0.001); n = n + 1); n", "3"],
		["n := 0; 3 repeat(i, wait(0.001); n = n + i); n", "3"],
		["list(1, 5, 7, 2) select(v, wait(0.001); v > 3)", "list(5, 7)"],
		["list(1, 5, 7, 2) selectInPlace(i, v, wait(0.001); i > 1)", "list(7, 2)"],
		['r := list(); "abc" foreach(i, c, wait(0.001); r append(c)); r', "list(97, 98, 99)"],
		["o := Object clone; k := 4; (o lexicalDo(wait(0.001); x := k * 2)) x", "8"],
		["o protos size", "1"],
		['"a #{wait(0.001); 1} b #{wait(0.001); 2}" interpolate', "a 1 b 2"],
		['s := "n=#{wait(0.001); 5}" asMutable; s interpolateInPlace; s', "n=5"],
	];
	for (const [code, expected] of cases) {
		const { output } = await io.eval(code);
		assert.equal(output, `==> ${expected}\n`, code);
	}
});

test("the Io-side handle table grows past its first 1024 slots", async () => {
	// 3000 Promises pending at once, each holding an Io-side handle until it settles
	globalThis.laterIndex = (i) => new Promise((resolve) => setTimeout(() => resolve(i), 5));
	const { output } = await io.eval(
		"futures := list(); 3000 repeat(i, futures append(JS laterIndex(i))); sum := 0; futures foreach(f, sum = sum + f await); sum");
	assert.equal(output, `==> ${(2999 * 3000) / 2}\n`);
	// and 3000 Io objects held by JS at once
	const held = [];
	globalThis.hold = (o) => { held.push(o); };
	await io.eval("3000 repeat(i, o := Object clone; o index := i; JS hold(o)); nil");
	assert.equal(held.length, 3000);
	assert.equal(held[0].index(), 0);
	assert.equal(held[2999].index(), 2999);
	held.length = 0;
});

test("a Promise can be used again after a collection", async () => {
	const result = await io.eval(
		"p := JS later(7); p then(block(v, Lobby thenA := v)); Collector collect; p then(block(v, Lobby thenB := v)); wait(0.06); list(thenA, thenB)");
	assert.equal(result.output, "==> list(7, 7)\n");
});

test("ticks, evals and JS calls leave nothing behind", async () => {
	const coroutines = async () => Number((await io.eval(
		'Collector collect; n := 0; Collector allObjects foreach(o, if(getSlot("o") type == "Coroutine", n = n + 1)); n')).output.slice(4));
	await io.eval("Lobby twice := method(x, x * 2)");
	const lobby = io.lobby;
	const start = await coroutines();
	for (let i = 0; i < 2000; i++) io.pump();
	for (let i = 0; i < 500; i++) await io.eval("1 + 1");
	for (let i = 0; i < 2000; i++) lobby.twice(i);
	const end = await coroutines();
	assert.ok(end - start < 20, `coroutine objects grew from ${start} to ${end}`);
});

test("an eval nothing can wake fails instead of hanging", async () => {
	const { status, output } = await io.eval('Coroutine currentCoroutine pause; "never"');
	assert.equal(status, 1);
	assert.match(output, /nothing left to resume/);
	assert.equal((await io.eval("1 + 1")).output, "==> 2\n");
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
