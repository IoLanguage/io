#!/usr/bin/env node
// Headless browser test runner for Io WASM tests
// Usage: node browser/run_tests.mjs
// Requires: playwright (npm install playwright)

import { createServer } from "http";
import { readFileSync, existsSync } from "fs";
import { join, extname } from "path";
import { chromium } from "playwright";

const BROWSER_DIR = join(import.meta.dirname);
const PORT = 8787; // unlikely to conflict
const TIMEOUT = 30000;

const MIME = {
	".html": "text/html",
	".js": "application/javascript",
	".wasm": "application/wasm",
};

// Minimal static file server
function startServer() {
	const server = createServer((req, res) => {
		const filePath = join(BROWSER_DIR, req.url === "/" ? "index.html" : req.url);
		if (!existsSync(filePath)) {
			res.writeHead(404);
			res.end("Not found");
			return;
		}
		const ext = extname(filePath);
		res.writeHead(200, { "Content-Type": MIME[ext] || "application/octet-stream" });
		res.end(readFileSync(filePath));
	});

	return new Promise((resolve) => {
		server.listen(PORT, () => resolve(server));
	});
}

// Check that WASM exists
if (!existsSync(join(BROWSER_DIR, "io_browser.wasm"))) {
	console.error("Error: browser/io_browser.wasm not found. Run 'make browser' first.");
	process.exit(1);
}

const server = await startServer();
const browser = await chromium.launch();

try {
	const page = await browser.newPage();

	page.on("pageerror", (err) => console.error("Page error:", err.message));

	await page.goto(`http://localhost:${PORT}/test.html`);

	// Wait for summary element to have content (tests finished)
	await page.waitForFunction(
		() => document.getElementById("summary")?.textContent?.length > 0,
		{ timeout: TIMEOUT }
	);

	// Extract results
	const results = await page.evaluate(() => {
		const tests = [];
		for (const el of document.querySelectorAll(".test")) {
			const passed = !!el.querySelector(".pass");
			const name = el.querySelector(".label")?.textContent || "";
			const detail = el.querySelector(".detail")?.textContent || "";
			tests.push({ name, passed, detail });
		}
		const summary = document.getElementById("summary").textContent;
		return { tests, summary };
	});

	// Print results
	for (const t of results.tests) {
		const icon = t.passed ? "\x1b[32mPASS\x1b[0m" : "\x1b[31mFAIL\x1b[0m";
		console.log(`${icon} ${t.name}`);
		if (!t.passed && t.detail) {
			console.log(`     ${t.detail.replace(/\n/g, "\n     ")}`);
		}
	}

	const allPassed = results.tests.every((t) => t.passed);
	console.log(`\n${results.summary}`);
	process.exitCode = allPassed ? 0 : 1;
} catch (e) {
	console.error("Test runner error:", e.message);
	process.exitCode = 1;
} finally {
	await browser.close();
	server.close();
}
