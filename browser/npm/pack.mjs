// Stage the @iolanguage/io npm package in build/npm/.
//
//   node browser/npm/pack.mjs [tag]
//
// tag is an Io release tag such as 2026.04.20-native-final or
// 2026.03.06-stackless-alpha; it becomes the package version. Without one
// the version stays 0.0.0-dev. Run `make browser` first.

import { cp, mkdir, readFile, rm, writeFile } from "node:fs/promises";
import { pathToFileURL } from "node:url";

const root = new URL("../../", import.meta.url);
const out = new URL("build/npm/", root);
const from = (path) => new URL(path, root);

// Io tags are dates with optional suffixes; npm needs semver, which forbids
// leading zeros. Release tags drop the suffix (2026.04.20-native-final ->
// 2026.4.20); prerelease tags keep it (-> 2026.3.6-stackless-alpha).
export function versionForTag(tag) {
	const m = /^v?(\d+)\.(\d+)\.(\d+)(?:-([0-9A-Za-z.-]+))?$/.exec(tag);
	if (!m) throw new Error(`can't make an npm version from tag "${tag}"`);
	const [, major, minor, patch, suffix] = m;
	const base = [major, minor, patch].map(Number).join(".");
	return suffix && isPrerelease(tag) ? `${base}-${suffix}` : base;
}

export function isPrerelease(tag) {
	return /alpha|beta|rc|pre/.test(tag);
}

async function main() {
	const tag = process.argv[2];
	const pkg = JSON.parse(await readFile(from("browser/npm/package.json"), "utf8"));
	if (tag) pkg.version = versionForTag(tag);

	await rm(out, { recursive: true, force: true });
	await mkdir(new URL("src/", out), { recursive: true });

	// src/package.json only marks src/ as ESM inside the repo; the package's
	// own package.json does that here.
	await cp(from("browser/src/"), new URL("src/", out), {
		recursive: true,
		filter: (path) => !path.endsWith("package.json"),
	});
	await cp(from("browser/npm/index.d.ts"), new URL("src/index.d.ts", out));
	await cp(from("browser/npm/README.md"), new URL("README.md", out));
	await cp(from("LICENSE.txt"), new URL("LICENSE.txt", out));
	await cp(from("browser/io_browser.wasm"), new URL("io_browser.wasm", out));
	await writeFile(new URL("package.json", out), JSON.stringify(pkg, null, "\t") + "\n");

	console.log(`staged ${pkg.name}@${pkg.version} in build/npm/`);
}

if (import.meta.url === pathToFileURL(process.argv[1]).href) await main();
