import { ContentBase } from "./ContentBase.js";
import { parseMarkdown } from "./MarkdownParser.js";

export class PageIndex {
    constructor () {
        this.json = null;
        this.isRoot = false;
        this.parentTitle = null;
        this.children = [];
    }

    async init () {
        await this.loadPage({
            isRoot: () => {
                const rootDir = new URL("../..", import.meta.url).href;
                const pageDir = new URL("./", window.location.href).href;
                return pageDir === rootDir;
            },
            pathSegments: () => {
                const segs = window.location.pathname.split("/").filter(s => s.length > 0);
                if (segs.length && segs[segs.length - 1].includes(".")) segs.pop();
                return segs;
            },
            urlParam: (key) => new URLSearchParams(window.location.search).get(key)
        });

        // Render to DOM
        this.render();
    }

    async loadPage (context) {
        this.isRoot = context.isRoot();
        const pathSegments = context.pathSegments();

        // Fetch _index.json, fall back to _index.md
        try {
            const resp = await ContentBase.asyncFetch("./_index.json");
            if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
            this.json = await resp.json();
        } catch (e) {
            try {
                const mdResp = await ContentBase.asyncFetch("./_index.md");
                if (!mdResp.ok) throw new Error(`HTTP ${mdResp.status}`);
                const mdText = await mdResp.text();
                this.json = parseMarkdown(mdText);
            } catch (e2) {
                console.error("Failed to load _index.json or _index.md:", e2);
                return;
            }
        }

        // Derive title from json or folder name
        const folderName = pathSegments.length > 0
            ? decodeURIComponent(pathSegments[pathSegments.length - 1])
            : "";
        this.pageTitle = ("title" in this.json) ? this.json.title : (folderName || "Untitled");
        this.topTitle = this.json.topTitle || "Io";

        // Check for back-link override via URL parameter
        this.backUrl = context.urlParam("back") || null;
        this.backTitle = null;

        // Fetch parent metadata for back-link and inherited topTitle
        if (!this.isRoot) {
            let parentJson = null;

            // If back URL is provided, fetch its metadata for the title
            if (this.backUrl) {
                const backDir = this.backUrl.replace(/\/[^/]*$/, "/");
                try {
                    const resp = await ContentBase.asyncFetch(`${backDir}_index.json`);
                    if (resp.ok) {
                        const json = await resp.json();
                        this.backTitle = json.title || null;
                    }
                } catch (e) { /* fall back */ }
                if (!this.backTitle) {
                    try {
                        const resp = await ContentBase.asyncFetch(`${backDir}_index.md`);
                        if (resp.ok) {
                            const json = parseMarkdown(await resp.text());
                            this.backTitle = json.title || null;
                        }
                    } catch (e) { /* fall back */ }
                }
            }

            try {
                const parentResp = await ContentBase.asyncFetch("../_index.json");
                if (parentResp.ok) {
                    parentJson = await parentResp.json();
                    this.parentTitle = parentJson.title || null;
                }
            } catch (e) { /* fall back */ }

            if (!this.parentTitle) {
                try {
                    const parentMd = await ContentBase.asyncFetch("../_index.md");
                    if (parentMd.ok) {
                        const mdText = await parentMd.text();
                        parentJson = parseMarkdown(mdText);
                        this.parentTitle = parentJson.title || null;
                    }
                } catch (e) { /* fall back */ }
            }

            // Inherit topTitle by walking up the directory tree
            if (!this.json.topTitle) {
                if (parentJson && parentJson.topTitle) {
                    this.topTitle = parentJson.topTitle;
                } else {
                    let depth = 2;
                    const maxDepth = pathSegments.length + 1;
                    while (depth <= maxDepth) {
                        const prefix = "../".repeat(depth);
                        try {
                            const resp = await ContentBase.asyncFetch(`${prefix}_index.json`);
                            if (resp.ok) {
                                const json = await resp.json();
                                if (json.topTitle) { this.topTitle = json.topTitle; break; }
                            }
                        } catch (e) { /* continue */ }
                        depth++;
                    }
                }
            }

            if (!this.parentTitle) {
                const parentSegments = [...pathSegments];
                parentSegments.pop();
                this.parentTitle = parentSegments.length > 0
                    ? decodeURIComponent(parentSegments[parentSegments.length - 1])
                    : "Home";
            }
        }

        // Create content children
        if (Array.isArray(this.json.content)) {
            this.children = this.json.content.map(c => ContentBase.fromJson(c, 0));
        }

        // Resolve (async fetches)
        await Promise.all(this.children.map(c => c.resolve()));
    }

    computePageHtml () {
        const heroImage = this.json.heroImage;

        // Header
        let headerHtml = '<div class="header">';
        if (this.isRoot) {
            if (this.pageTitle) headerHtml += `<h1>${this.pageTitle}</h1>`;
            headerHtml += `<div class="brand">${this.topTitle}</div>`;
        } else {
            const backHref = this.backUrl || "../index.html";
            const backLabel = this.backTitle || this.parentTitle;
            headerHtml += "<div>";
            if (!heroImage) headerHtml += `<h1>${this.pageTitle}</h1>`;
            headerHtml += `<a class="back-link" href="${backHref}">&larr; ${backLabel}</a>`;
            headerHtml += "</div>";
            headerHtml += `<a class="brand" href="${backHref}">${this.topTitle}</a>`;
        }
        headerHtml += "</div>";

        // Hero (large title + background image)
        let heroHtml = "";
        if (heroImage && !this.isRoot) {
            const encoded = encodeURI(heroImage).replace(/'/g, "%27");
            heroHtml = `<div class="hero" style="background-image: url('${encoded}')">`;
            heroHtml += `<h1 class="hero-title">${this.pageTitle}</h1>`;
            heroHtml += "</div>";
        }

        // Intro
        let introHtml = "";
        if (this.json.subtitle) {
            introHtml = `<p class="intro">${this.json.subtitle}</p>`;
        }

        // Content
        const contentHtml = this.children.map(c => c.computeHtml()).join("");

        return headerHtml + heroHtml + introHtml + contentHtml;
    }

    computeDocumentTitle () {
        if (this.isRoot) {
            return this.topTitle;
        }
        return `${this.pageTitle} \u2013 ${this.topTitle}`;
    }

    render () {
        const page = document.querySelector(".page");
        if (!page) return;

        // Skip HTML generation if static content was already inlined by the build
        if (!page.classList.contains("loaded")) {
            page.innerHTML = this.computePageHtml();
        }

        if (this.json.pageLayout) {
            page.classList.add(`page-${this.json.pageLayout}`);
        }
        page.classList.add("loaded");

        // Post-render hooks
        this.children.forEach(c => c.postRender(page));

        // Document title
        document.title = this.computeDocumentTitle();
    }
}
