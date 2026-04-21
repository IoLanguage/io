#!/usr/bin/env node
// Walk libs/ for .c and .io sources, extract //metadoc, //doc, and /*doc ... */
// comments, and regenerate docs/Reference/ from them.

import { readFileSync, readdirSync, writeFileSync, mkdirSync, rmSync, statSync } from "node:fs";
import { join, dirname, basename } from "node:path";
import { fileURLToPath } from "node:url";

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = process.argv[2] || join(HERE, "..");
// Only scan iovm; basekit / garbagecollector protos are C-level data
// structures, not user-facing Io protos.
const LIBS = join(ROOT, "libs", "iovm");
const OUT = join(ROOT, "docs", "Reference");

const SKIP_DIRS = new Set(["_old", "_test", "tests"]);
const SKIP_FILES = new Set(["IoVMInit.c"]); // generated — duplicates .io docs

function walk (dir) {
    const out = [];
    for (const entry of readdirSync(dir, { withFileTypes: true })) {
        if (entry.name.startsWith(".")) continue;
        const full = join(dir, entry.name);
        if (entry.isDirectory()) {
            if (SKIP_DIRS.has(entry.name)) continue;
            out.push(...walk(full));
        } else if ((entry.name.endsWith(".c") || entry.name.endsWith(".io")) && !SKIP_FILES.has(entry.name)) {
            out.push(full);
        }
    }
    return out;
}

// Extract doc entries from a source file.
// Returns { proto, kind: "metadoc"|"doc", name, body, source }
function parseFile (path) {
    const text = readFileSync(path, "utf-8");
    const entries = [];

    // Single-line: //metadoc Proto field rest...  or  //doc Proto slot rest...
    // Accepts "//metadoc" or "// metadoc".
    const singleLine = /^[ \t]*\/\/[ \t]*(metadoc|doc)[ \t]+(\S+)[ \t]+(\S+)[ \t]+(.+?)[ \t]*$/gm;
    let m;
    while ((m = singleLine.exec(text)) !== null) {
        const [, kind, proto, name, body] = m;
        entries.push({ kind, proto, name, body: body.trim(), source: path });
    }

    // Multi-line: /*doc Proto signature...\n  body...\n*/
    const metaFields = new Set(["description", "category", "module", "license", "copyright", "author"]);
    const multiLine = /\/\*[ \t]*doc[ \t]+(\S+)[ \t]+([^\n*][^\n]*)\n([\s\S]*?)\*\//g;
    while ((m = multiLine.exec(text)) !== null) {
        const [, proto, signature, rawBody] = m;
        const name = signature.trim();
        const body = rawBody
            .split("\n")
            .map(l => l.replace(/^[ \t]+/, "").replace(/[ \t]+$/, ""))
            .join("\n")
            .trim();
        const kind = metaFields.has(name) ? "metadoc" : "doc";
        entries.push({ kind, proto, name, body, source: path });
    }

    // Multi-line metadoc: /*metadoc Proto field\n  body...\n*/
    const multiMeta = /\/\*[ \t]*metadoc[ \t]+(\S+)[ \t]+(\S+)[ \t]*\n([\s\S]*?)\*\//g;
    while ((m = multiMeta.exec(text)) !== null) {
        const [, proto, field, rawBody] = m;
        const body = rawBody
            .split("\n")
            .map(l => l.replace(/^[ \t]+/, "").replace(/[ \t]+$/, ""))
            .join("\n")
            .trim();
        entries.push({ kind: "metadoc", proto, name: field, body, source: path });
    }

    return entries;
}

// Collect every doc entry.
const files = walk(LIBS);
const byProto = new Map(); // proto -> { metadoc: Map<field, value>, slots: Map<name, {body, sources[]}> }

for (const file of files) {
    for (const entry of parseFile(file)) {
        if (!byProto.has(entry.proto)) {
            byProto.set(entry.proto, { metadoc: new Map(), slots: new Map() });
        }
        const bucket = byProto.get(entry.proto);
        if (entry.kind === "metadoc") {
            bucket.metadoc.set(entry.name, entry.body);
        } else {
            if (!bucket.slots.has(entry.name)) {
                bucket.slots.set(entry.name, { body: entry.body, sources: [entry.source] });
            } else {
                const existing = bucket.slots.get(entry.name);
                if (!existing.sources.includes(entry.source)) existing.sources.push(entry.source);
                if (entry.body.length > existing.body.length) existing.body = entry.body;
            }
        }
    }
}

// Filter out anything that looks like junk: proto names should start with a
// letter and contain only word chars.
for (const proto of [...byProto.keys()]) {
    if (!/^[A-Za-z][A-Za-z0-9_]*$/.test(proto)) byProto.delete(proto);
}

// Drop VM-internal C structures: protos whose only metadoc is license/copyright,
// with no description and no slot docs. These are C data-structure headers
// (PHash, Tag, Token, etc.) that happen to use the metadoc syntax but are not
// user-facing Io protos.
for (const [proto, bucket] of [...byProto.entries()]) {
    const hasDescription = bucket.metadoc.has("description");
    const hasSlots = bucket.slots.size > 0;
    if (!hasDescription && !hasSlots) byProto.delete(proto);
}

// Group by category.
const byCategory = new Map();
for (const [proto, bucket] of byProto) {
    const category = (bucket.metadoc.get("category") || "Uncategorized").trim();
    if (!byCategory.has(category)) byCategory.set(category, []);
    byCategory.get(category).push(proto);
}

const categories = [...byCategory.keys()].sort((a, b) => {
    if (a === "Uncategorized") return 1;
    if (b === "Uncategorized") return -1;
    return a.localeCompare(b);
});

function slotSortKey (name) {
    // Sort by the bare identifier, ignoring (args) suffix.
    const m = name.match(/^([A-Za-z_][A-Za-z0-9_]*)/);
    return (m ? m[1] : name).toLowerCase();
}

function escapeMd (s) {
    return s.replace(/\|/g, "\\|");
}

function shellHtml (title, depth) {
    const up = "../".repeat(depth);
    return `<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>${title} – Io</title>
<link rel="stylesheet" href="${up}style/style.css">
<link rel="alternate" type="text/plain" title="llms.txt" href="/llms.txt">
</head>
<body>
<div class="page"></div>
<script src="${up}style/layout/layout.js" type="module"></script>
</body>
</html>
`;
}

// Wipe and recreate the Reference directory.
rmSync(OUT, { recursive: true, force: true });
mkdirSync(OUT, { recursive: true });

// Landing page — hierarchical list: category > proto.
let landing = "# Reference\n\n";
landing += "Auto-generated API reference extracted from `//doc`, `//metadoc`, and `/*doc ... */` comments in the Io source tree.\n\n";
for (const cat of categories) {
    landing += `- ${cat}\n`;
    for (const proto of byCategory.get(cat).sort()) {
        const encoded = encodeURIComponent(proto);
        landing += `  - [${proto}](${encoded}/index.html)\n`;
    }
}
landing += "\n";
writeFileSync(join(OUT, "_index.md"), landing);
writeFileSync(join(OUT, "index.html"), shellHtml("Reference", 2));

// Per-proto pages.
let totalSlots = 0;
for (const [proto, bucket] of byProto) {
    const dir = join(OUT, proto);
    mkdirSync(dir, { recursive: true });

    let page = `# ${proto}\n\n`;
    const desc = (bucket.metadoc.get("description") || "").trim();
    if (desc) page += `${desc}\n\n`;

    // Other metadoc fields surface as a small list. Hide boilerplate
    // (copyright/license/credits/author) and anything already rendered.
    const hiddenMeta = new Set(["description", "copyright", "license", "credits", "author", "category"]);
    const otherMeta = [...bucket.metadoc.entries()].filter(([k]) => !hiddenMeta.has(k));
    if (otherMeta.length > 0) {
        for (const [k, v] of otherMeta) {
            page += `- **${k}**: ${v.trim()}\n`;
        }
        page += "\n";
    }

    const slots = [...bucket.slots.entries()]
        .sort((a, b) => slotSortKey(a[0]).localeCompare(slotSortKey(b[0])));

    for (const [name, info] of slots) {
        page += `## ${name}\n\n`;
        if (info.body) page += `${info.body}\n\n`;
    }

    writeFileSync(join(dir, "_index.md"), page);
    writeFileSync(join(dir, "index.html"), shellHtml(proto, 3));
    totalSlots += slots.length;
}

console.log(`Reference: ${byProto.size} protos, ${totalSlots} slots, ${categories.length} categories.`);
