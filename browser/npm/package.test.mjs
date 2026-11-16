// Tests the staged npm package: packs build/npm/, installs the tarball into a
// scratch project and imports it by name, as a user would.
// Usage: make check-npm-package

import { test } from "node:test";
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { fileURLToPath } from "node:url";
import { versionForTag, isPrerelease } from "./pack.mjs";

const pkgDir = fileURLToPath(new URL("../../build/npm/", import.meta.url));

test("Io tags become semver versions", () => {
	assert.equal(versionForTag("2026.04.20-native-final"), "2026.4.20");
	assert.equal(versionForTag("2026.03.06-stackless-alpha"), "2026.3.6-stackless-alpha");
	assert.equal(versionForTag("2017.09.06"), "2017.9.6");
	assert.equal(versionForTag("v1.2.3"), "1.2.3");
	assert.throws(() => versionForTag("latest"), /can't make an npm version/);
	assert.equal(isPrerelease("2026.03.06-stackless-alpha"), true);
	assert.equal(isPrerelease("2026.04.20-native-final"), false);
});

test("the packed package installs and runs", async (t) => {
	const work = await mkdtemp(join(tmpdir(), "io-npm-"));
	t.after(() => rm(work, { recursive: true, force: true }));

	const npm = (args, cwd) => execFileSync("npm", args, { cwd, encoding: "utf8" });
	// npm 11 and earlier print an array; npm 12 an object keyed by package name
	const packOut = JSON.parse(npm(["pack", "--json", "--pack-destination", work], pkgDir));
	const { filename, files } = Array.isArray(packOut) ? packOut[0] : Object.values(packOut)[0];
	const packed = files.map((f) => f.path).sort();
	assert.deepEqual(packed, [
		"LICENSE.txt", "README.md", "io_browser.wasm", "package.json",
		"src/bridge.js", "src/index.d.ts", "src/index.js", "src/proxy.js",
		"src/serialize.js", "src/vm.js", "src/wasi.js",
	]);

	await writeFile(join(work, "package.json"), '{"private": true, "type": "module"}');
	npm(["install", "--no-audit", "--no-fund", "--offline", join(work, filename)], work);

	await writeFile(join(work, "smoke.js"), `
		import { Io } from "@iolanguage/io";
		const io = await Io.load();
		const sum = await io.eval("list(1, 2, 3) map(x, x * x) sum");
		globalThis.later = (v) => new Promise((r) => setTimeout(() => r(v), 5));
		const awaited = await io.eval("JS later(41) await + 1");
		console.log(JSON.stringify({ sum, awaited }));
	`);
	const result = JSON.parse(execFileSync("node", ["smoke.js"], { cwd: work, encoding: "utf8" }));
	assert.deepEqual(result, {
		sum: { status: 0, output: "==> 14\n" },
		awaited: { status: 0, output: "==> 42\n" },
	});

	const wasmPath = execFileSync("node", ["-p", 'require.resolve("@iolanguage/io/io_browser.wasm")'], { cwd: work, encoding: "utf8" }).trim();
	assert.ok((await readFile(wasmPath)).length > 0);
});
