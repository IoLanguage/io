#!/usr/bin/env node
// Benchmark runner — measures one set of results and appends an entry to
// bench/history.json. Run from anywhere; requires wasmtime on PATH and a
// built io binary.
//
// Env overrides (used by CI and backfills):
//   IO_BIN        path to the io wasm binary    (default: build/bin/io_static)
//   BENCH_COMMIT  commit hash to record          (default: git HEAD short hash)
//   BENCH_DATE    YYYY-MM-DD to record           (default: today)
//   BENCH_MACHINE machine label for the series   (default: CPU model string)

import { execFileSync, execSync } from "child_process";
import { readFileSync, writeFileSync, existsSync } from "fs";
import { fileURLToPath } from "url";
import { dirname, join } from "path";
import os from "os";

const BENCH_DIR = dirname(fileURLToPath(import.meta.url));
const ROOT = join(BENCH_DIR, "..");
const IO_BIN = process.env.IO_BIN || join(ROOT, "build", "bin", "io_static");
const HISTORY = join(BENCH_DIR, "history.json");

// Wall-clock seconds for one wasmtime invocation (includes ~30ms VM startup)
function timeRun(args) {
	const t0 = process.hrtime.bigint();
	execFileSync("wasmtime", ["--dir=.", IO_BIN, ...args], {
		cwd: ROOT,
		stdio: ["ignore", "ignore", "inherit"],
	});
	return Number(process.hrtime.bigint() - t0) / 1e9;
}

// Best of N runs — minimum is the least noisy estimator for wall time
function bestOf(n, args) {
	let best = Infinity;
	for (let i = 0; i < n; i++) best = Math.min(best, timeRun(args));
	return Number(best.toFixed(3));
}

console.log(`io binary: ${IO_BIN}`);

// --- micro benchmarks (ops/sec rates measured inside the VM) ---
const microOut = execFileSync("wasmtime", ["--dir=.", IO_BIN, "bench/micro.io"], {
	cwd: ROOT,
	encoding: "utf8",
});
const micro = JSON.parse(microOut.trim().split("\n").pop());
console.log("micro:", micro);

// --- macro benchmarks (wall-clock seconds, lower is better) ---
const MACROS = [
	["binarytrees10", ["samples/shootout/binarytrees.io", "10"]],
	["nbody50k", ["samples/shootout/nbody.io", "50000"]],
	["nsieve6", ["samples/shootout/nsieve.io", "6"]],
];

const metrics = { ...micro };
delete metrics.ioVersion;

for (const [name, args] of MACROS) {
	metrics[name] = bestOf(3, args);
	console.log(`${name}: ${metrics[name]}s`);
}

// --- build and append the entry ---
const entry = {
	date: process.env.BENCH_DATE || new Date().toISOString().slice(0, 10),
	commit:
		process.env.BENCH_COMMIT ||
		execSync("git rev-parse --short HEAD", { cwd: ROOT, encoding: "utf8" }).trim(),
	machine: process.env.BENCH_MACHINE || os.cpus()[0].model.trim(),
	ioVersion: micro.ioVersion,
	metrics,
};

const history = existsSync(HISTORY) ? JSON.parse(readFileSync(HISTORY, "utf8")) : [];
history.push(entry);
history.sort((a, b) => (a.date < b.date ? -1 : a.date > b.date ? 1 : 0));
writeFileSync(HISTORY, JSON.stringify(history, null, "\t") + "\n");
console.log(`recorded ${entry.commit} (${entry.date}) on "${entry.machine}" — ${history.length} entries total`);
