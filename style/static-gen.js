#!/usr/bin/env node

/**
 * Static HTML generator for the STRVCT layout engine.
 *
 * Walks the site tree, finds pages that use the layout engine (index.html with
 * a sibling _index.json or _index.md), and injects pre-rendered HTML into each
 * page. This makes content readable by AI crawlers and other clients that don't
 * execute JavaScript.
 *
 * The browser-side layout.js still runs and overwrites the static content on
 * load, so the two paths produce identical results.
 *
 * Usage:  node strvct/style/static-gen.js
 * Run from the site root (Servers/GameServer/site/).
 */

import { readFileSync, writeFileSync, existsSync, readdirSync } from "node:fs";
import { join, relative, dirname, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";

// Layout engine imports — same modules the browser uses
import { ContentBase } from "./layout/ContentBase.js";
import { ContentText } from "./layout/ContentText.js";
import { ContentCards } from "./layout/ContentCards.js";
import { ContentKeyValue } from "./layout/ContentKeyValue.js";
import { ContentUnorderedList } from "./layout/ContentUnorderedList.js";
import { ContentOrderedList } from "./layout/ContentOrderedList.js";
import { ContentTable } from "./layout/ContentTable.js";
import { ContentImage } from "./layout/ContentImage.js";
import { ContentTimeline } from "./layout/ContentTimeline.js";
import { ContentToc } from "./layout/ContentToc.js";
import { ContentFAQ } from "./layout/ContentFAQ.js";
import { PageIndex } from "./layout/PageIndex.js";
import { parseMarkdown } from "./layout/MarkdownParser.js";

// Register content types (mirrors layout.js)
ContentBase.typeMap = {
    ContentText,
    ContentCards,
    ContentKeyValue,
    ContentUnorderedList,
    ContentOrderedList,
    ContentTable,
    ContentImage,
    ContentTimeline,
    ContentToc,
    ContentFAQ,
};

const __dirname = dirname(fileURLToPath(import.meta.url));
const siteRoot = resolve(__dirname, ".."); // strvct root

const skipDirs = new Set([
    "node_modules", "external-libs", "source", "build",
    "npm-pkg", "webserver", "resources",
]);

// ---------------------------------------------------------------------------
// Filesystem-backed fetch that resolves URLs relative to a page directory
// ---------------------------------------------------------------------------

function createFetchFn (pageDir) {
    return async function (url) {
        const decoded = decodeURIComponent(url);
        const filePath = resolve(pageDir, decoded);
        try {
            const content = readFileSync(filePath, "utf-8");
            return {
                ok: true,
                status: 200,
                json: async () => JSON.parse(content),
                text: async () => content,
            };
        } catch {
            return { ok: false, status: 404 };
        }
    };
}

// ---------------------------------------------------------------------------
// Directory walker — finds pages that have both index.html and _index content
// ---------------------------------------------------------------------------

function findLayoutPages (dir) {
    const pages = [];
    let entries;
    try {
        entries = readdirSync(dir, { withFileTypes: true });
    } catch {
        return pages;
    }

    const hasIndex = entries.some(e => e.name === "index.html" && e.isFile());
    const hasContent = entries.some(e =>
        (e.name === "_index.json" || e.name === "_index.md") && e.isFile()
    );

    if (hasIndex && hasContent) {
        pages.push(dir);
    }

    for (const entry of entries) {
        if (entry.isDirectory() && !entry.name.startsWith(".") && !skipDirs.has(entry.name)) {
            pages.push(...findLayoutPages(join(dir, entry.name)));
        }
    }

    return pages;
}

// ---------------------------------------------------------------------------
// Generate static HTML for a single page
// ---------------------------------------------------------------------------

async function generatePage (pageDir) {
    const relPath = relative(siteRoot, pageDir) || ".";
    const pathSegments = relPath === "."
        ? []
        : relPath.split(sep).filter(s => s.length > 0);

    ContentBase.setFetchFn(createFetchFn(pageDir));

    const context = {
        isRoot: () => pageDir === siteRoot,
        pathSegments: () => pathSegments,
        urlParam: () => null,
    };

    const page = new PageIndex();
    await page.loadPage(context);

    if (!page.json) {
        console.log(`  skipped (no content): ${relPath}`);
        return;
    }

    const pageHtml = page.computePageHtml();
    const docTitle = page.computeDocumentTitle();

    const pageClasses = ["page", "loaded"];
    if (page.json.pageLayout) {
        pageClasses.push(`page-${page.json.pageLayout}`);
    }

    // Read existing HTML template
    const htmlPath = join(pageDir, "index.html");
    let html = readFileSync(htmlPath, "utf-8");

    // Replace the .page div contents, or insert one if missing
    const pageDiv = `<div class="${pageClasses.join(" ")}">${pageHtml}</div>`;
    if (/(<body>\s*)<div class="page[^"]*"[^>]*>[\s\S]*<\/div>(\s*<script)/.test(html)) {
        html = html.replace(
            /(<body>\s*)<div class="page[^"]*"[^>]*>[\s\S]*<\/div>(\s*<script)/,
            `$1${pageDiv}$2`
        );
    } else {
        html = html.replace(
            /(<body>)\s*(\s*<script)/,
            `$1\n  ${pageDiv}\n$2`
        );
    }

    // Add or update <title> tag
    if (/<title>/.test(html)) {
        html = html.replace(/<title>[^<]*<\/title>/, `<title>${docTitle}</title>`);
    } else {
        html = html.replace("</head>", `  <title>${docTitle}</title>\n</head>`);
    }

    // Inject a <link rel="alternate"> pointing to /llms.txt so LLM agents
    // can discover the machine-readable index from any page.
    const llmsLinkTag = '<link rel="alternate" type="text/plain" title="llms.txt" href="/llms.txt">';
    if (!html.includes('href="/llms.txt"') && /<\/head>/i.test(html)) {
        html = html.replace(/<\/head>/i, `  ${llmsLinkTag}\n</head>`);
    }

    writeFileSync(htmlPath, html);
    console.log(`  generated: ${relPath}`);
}

// ---------------------------------------------------------------------------
// Sitemap generator
// ---------------------------------------------------------------------------

function generateSitemap (pages, baseUrl) {
    let xml = '<?xml version="1.0" encoding="UTF-8"?>\n';
    xml += '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n';

    for (const pageDir of pages) {
        const relPath = relative(siteRoot, pageDir);
        const urlPath = relPath
            ? relPath.split(sep).map(s => encodeURIComponent(s)).join("/") + "/"
            : "";
        xml += `  <url><loc>${baseUrl}${urlPath}</loc></url>\n`;
    }

    xml += `  <url><loc>${baseUrl}llms.txt</loc></url>\n`;
    xml += `  <url><loc>${baseUrl}llms-full.txt</loc></url>\n`;
    xml += "</urlset>\n";
    return xml;
}

// ---------------------------------------------------------------------------
// llms.txt / llms-full.txt generation
// ---------------------------------------------------------------------------

function htmlToPlain (html) {
    if (!html) return "";
    return html
        .replace(/<br\s*\/?>/gi, " ")
        .replace(/<[^>]+>/g, "")
        .replace(/&nbsp;/g, " ")
        .replace(/&mdash;/g, "\u2014")
        .replace(/&ndash;/g, "\u2013")
        .replace(/&ldquo;/g, "\u201c").replace(/&rdquo;/g, "\u201d")
        .replace(/&lsquo;/g, "\u2018").replace(/&rsquo;/g, "\u2019")
        .replace(/&amp;/g, "&")
        .replace(/&lt;/g, "<")
        .replace(/&gt;/g, ">")
        .replace(/\s+/g, " ")
        .trim();
}

function htmlToMarkdown (html) {
    if (!html) return "";
    let s = html;
    // Code blocks
    s = s.replace(/<pre><code>([\s\S]*?)<\/code><\/pre>/gi, (_, code) => `\n\`\`\`\n${code}\n\`\`\`\n`);
    // Headings (rare in bodies, but handle them)
    s = s.replace(/<h([1-6])[^>]*>([\s\S]*?)<\/h\1>/gi, (_, level, text) => `\n\n${"#".repeat(+level)} ${text.trim()}\n\n`);
    // Lists: open tag → newline; close tag → newline; items become "- item"
    s = s.replace(/<\/li>\s*<li[^>]*>/gi, "\n- ");
    s = s.replace(/<(ul|ol)[^>]*>\s*<li[^>]*>/gi, "\n- ");
    s = s.replace(/<\/li>\s*<\/(ul|ol)>/gi, "\n");
    s = s.replace(/<(ul|ol)[^>]*>/gi, "\n").replace(/<\/(ul|ol)>/gi, "\n");
    s = s.replace(/<li[^>]*>/gi, "- ").replace(/<\/li>/gi, "\n");
    // Inline formatting
    s = s.replace(/<a\s+[^>]*href="([^"]+)"[^>]*>([\s\S]*?)<\/a>/gi, "[$2]($1)");
    s = s.replace(/<(strong|b)>([\s\S]*?)<\/(strong|b)>/gi, "**$2**");
    s = s.replace(/<(em|i)>([\s\S]*?)<\/(em|i)>/gi, "*$2*");
    s = s.replace(/<code>([\s\S]*?)<\/code>/gi, "`$1`");
    // Paragraphs and breaks
    s = s.replace(/<p[^>]*>/gi, "").replace(/<\/p>/gi, "\n\n");
    s = s.replace(/<br\s*\/?>/gi, "\n");
    // Strip remaining tags
    s = s.replace(/<[^>]+>/g, "");
    // Entities
    s = s.replace(/&nbsp;/g, " ")
        .replace(/&mdash;/g, "\u2014")
        .replace(/&ndash;/g, "\u2013")
        .replace(/&ldquo;/g, "\u201c").replace(/&rdquo;/g, "\u201d")
        .replace(/&lsquo;/g, "\u2018").replace(/&rsquo;/g, "\u2019")
        .replace(/&amp;/g, "&")
        .replace(/&lt;/g, "<")
        .replace(/&gt;/g, ">");
    s = s.replace(/\n{3,}/g, "\n\n");
    return s.trim();
}

function pageUrlPath (pageDir) {
    const relPath = relative(siteRoot, pageDir);
    if (!relPath) return "";
    return relPath.split(sep).map(s => encodeURIComponent(s)).join("/") + "/";
}

function readPageMeta (pageDir) {
    const jsonPath = join(pageDir, "_index.json");
    const mdPath = join(pageDir, "_index.md");

    if (existsSync(jsonPath)) {
        const json = JSON.parse(readFileSync(jsonPath, "utf-8"));
        return { json, rawMarkdown: null };
    }
    if (existsSync(mdPath)) {
        const raw = readFileSync(mdPath, "utf-8");
        return { json: parseMarkdown(raw), rawMarkdown: raw };
    }
    return null;
}

function pageDisplayTitle (pageDir, meta) {
    if (meta && meta.json && meta.json.title) return meta.json.title;
    const relPath = relative(siteRoot, pageDir);
    return relPath ? relPath.split(sep).pop() : "";
}

function contentEntryToMarkdown (entry, depth = 0) {
    const title = entry.title || "";
    const heading = depth === 0 ? "##" : "###";

    switch (entry.type) {
        case "ContentText": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            if (entry.body) md += htmlToMarkdown(entry.body) + "\n\n";
            if (Array.isArray(entry.content)) {
                for (const child of entry.content) md += contentEntryToMarkdown(child, depth + 1);
            }
            return md;
        }
        case "ContentCards": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            for (const item of entry.items || []) {
                if (typeof item === "string") {
                    md += `- ${item}\n`;
                } else {
                    let line = `- **${item.title || item.folder || ""}**`;
                    if (item.href) line = `- [${item.title || item.href}](${item.href})`;
                    if (item.date) line += ` (${item.date})`;
                    if (item.subtitle) line += ` — ${htmlToPlain(item.subtitle)}`;
                    md += line + "\n";
                }
            }
            return md + "\n";
        }
        case "ContentKeyValue": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            for (const [k, v] of Object.entries(entry.items || {})) {
                md += `- **${k}**: ${htmlToPlain(String(v))}\n`;
            }
            return md + "\n";
        }
        case "ContentUnorderedList":
        case "ContentOrderedList": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            const marker = entry.type === "ContentOrderedList" ? (i) => `${i + 1}.` : () => "-";
            (entry.items || []).forEach((item, i) => {
                const text = typeof item === "string" ? item : (item.text || "");
                md += `${marker(i)} ${htmlToPlain(text)}\n`;
            });
            return md + "\n";
        }
        case "ContentTable": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            if (entry.subtitle) md += htmlToPlain(entry.subtitle) + "\n\n";
            const cols = entry.columns || [];
            const rows = entry.rows || [];
            if (cols.length) {
                md += "| " + cols.join(" | ") + " |\n";
                md += "| " + cols.map(() => "---").join(" | ") + " |\n";
                for (const row of rows) {
                    md += "| " + row.map(c => Array.isArray(c) ? c.join("; ") : htmlToPlain(String(c || ""))).join(" | ") + " |\n";
                }
            }
            if (entry.note) md += `\n*${htmlToPlain(entry.note)}*\n`;
            return md + "\n";
        }
        case "ContentTimeline": {
            let md = "";
            if (title) md += `${heading} ${title}\n\n`;
            for (const item of entry.items || []) {
                const date = item.date ? `**${item.date}** — ` : "";
                const t = item.title || "";
                const sub = item.subtitle ? `: ${htmlToPlain(item.subtitle)}` : "";
                md += `- ${date}${t}${sub}\n`;
            }
            return md + "\n";
        }
        case "ContentImage": {
            const alt = entry.alt || entry.title || "";
            const src = entry.src || entry.href || "";
            return src ? `![${alt}](${src})\n\n` : "";
        }
        case "ContentToc":
            return "";
        default:
            return "";
    }
}

function jsonPageToMarkdown (json) {
    let md = "";
    if (json.title) md += `# ${json.title}\n\n`;
    if (json.subtitle) md += htmlToPlain(json.subtitle) + "\n\n";
    for (const entry of json.content || []) {
        md += contentEntryToMarkdown(entry, 0);
    }
    return md.trim() + "\n";
}

function groupOrderKey (group) {
    // Keep a stable, readable ordering: docs first, then alphabetical.
    if (group === "docs") return "0";
    return "1" + group.toLowerCase();
}

function generateLlmsIndex (pages) {
    const rootMeta = readPageMeta(siteRoot);
    const rootJson = rootMeta ? rootMeta.json : {};
    const rootTitle = rootJson.topTitle || rootJson.title || "Site";
    const rootSubtitle = htmlToPlain(rootJson.subtitle || "");

    const groups = new Map();
    for (const pageDir of pages) {
        if (pageDir === siteRoot) continue;
        const relPath = relative(siteRoot, pageDir);
        const top = relPath.split(sep)[0];
        if (!groups.has(top)) groups.set(top, []);
        groups.get(top).push(pageDir);
    }

    const sortedGroups = [...groups.keys()].sort((a, b) =>
        groupOrderKey(a).localeCompare(groupOrderKey(b))
    );

    let txt = `# ${rootTitle}\n\n`;
    if (rootSubtitle) txt += `> ${rootSubtitle}\n\n`;

    for (const group of sortedGroups) {
        const groupDir = join(siteRoot, group);
        const groupMeta = readPageMeta(groupDir);
        const groupTitle = (groupMeta && groupMeta.json && groupMeta.json.title) || group;
        txt += `## ${groupTitle}\n\n`;

        const groupPages = groups.get(group).sort((a, b) =>
            relative(siteRoot, a).localeCompare(relative(siteRoot, b))
        );

        for (const pageDir of groupPages) {
            const meta = readPageMeta(pageDir);
            if (!meta) continue;
            const title = pageDisplayTitle(pageDir, meta);
            const subtitle = htmlToPlain(meta.json.subtitle || "");
            const url = "/" + pageUrlPath(pageDir);
            txt += `- [${title}](${url})`;
            if (subtitle) txt += `: ${subtitle}`;
            txt += "\n";
        }
        txt += "\n";
    }

    return txt;
}

function rootFirstParagraph () {
    const meta = readPageMeta(siteRoot);
    if (!meta || !meta.json || !meta.json.content) return "";
    for (const entry of meta.json.content) {
        if (entry.type === "ContentText" && entry.body) {
            const match = entry.body.match(/<p[^>]*>([\s\S]*?)<\/p>/i);
            if (match) return htmlToPlain(match[0]);
        }
    }
    return "";
}

function generateLlmsFull (pages) {
    const rootMeta = readPageMeta(siteRoot);
    const rootJson = rootMeta ? rootMeta.json : {};
    const rootTitle = rootJson.topTitle || rootJson.title || "Site";

    const sorted = [...pages].sort((a, b) => {
        if (a === siteRoot) return -1;
        if (b === siteRoot) return 1;
        return relative(siteRoot, a).localeCompare(relative(siteRoot, b));
    });

    let out = `# ${rootTitle} — Full Documentation\n\n`;
    const summary = rootFirstParagraph();
    if (summary) out += `${summary}\n\n`;
    out += "See also: /llms.txt (curated index) and /sitemap.xml.\n\n";

    for (const pageDir of sorted) {
        const meta = readPageMeta(pageDir);
        if (!meta) continue;
        const urlPath = pageUrlPath(pageDir);
        const url = "/" + urlPath;
        const pathLabel = relative(siteRoot, pageDir) || "(root)";

        out += "\n\n---\n\n";
        out += `<!-- Page: ${pathLabel} -->\n`;
        out += `Source: ${url}\n\n`;

        if (meta.rawMarkdown) {
            out += meta.rawMarkdown.trim() + "\n";
        } else {
            out += jsonPageToMarkdown(meta.json);
        }
    }

    return out;
}

// ---------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------

function loadConfig () {
    const configPath = join(siteRoot, "llms-config.json");
    if (existsSync(configPath)) {
        return JSON.parse(readFileSync(configPath, "utf-8"));
    }
    return {};
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

async function main () {
    const config = loadConfig();
    const siteUrl = config.siteUrl || "/";

    const pages = findLayoutPages(siteRoot);
    console.log(`Static gen: found ${pages.length} layout pages under ${siteRoot}`);

    for (const pageDir of pages) {
        await generatePage(pageDir);
    }

    // Write sitemap.xml at site root
    const sitemap = generateSitemap(pages, siteUrl);
    writeFileSync(join(siteRoot, "sitemap.xml"), sitemap);
    console.log(`  generated: sitemap.xml (${pages.length} URLs)`);

    // Write llms.txt (curated index for LLMs)
    const llmsIndex = generateLlmsIndex(pages);
    writeFileSync(join(siteRoot, "llms.txt"), llmsIndex);
    console.log("  generated: llms.txt");

    // Write llms-full.txt (full content for LLMs)
    const llmsFull = generateLlmsFull(pages);
    writeFileSync(join(siteRoot, "llms-full.txt"), llmsFull);
    console.log("  generated: llms-full.txt");

    console.log(`Static gen: done (${pages.length} pages).`);
}

main().catch(e => {
    console.error("Static gen failed:", e);
    process.exit(1);
});
