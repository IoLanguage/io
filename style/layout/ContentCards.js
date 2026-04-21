import { ContentBase } from "./ContentBase.js";
import { parseMarkdown } from "./MarkdownParser.js";

export class ContentCards extends ContentBase {
    constructor () {
        super();
        this.resolvedItems = [];
    }

    static encodePath (path) {
        return path.split("/").map(s => encodeURIComponent(s)).join("/");
    }

    static cardSubtitle (meta) {
        if (meta.cardSubtitle) return meta.cardSubtitle;
        const s = meta.subtitle || "";
        const br = s.indexOf("<br");
        return br !== -1 ? s.slice(0, br) : s;
    }

    async resolve () {
        const items = this.json.items || [];
        this.resolvedItems = [];

        const promises = items.map(async (item) => {
            if (typeof item === "string") {
                const folder = item;
                const encoded = ContentCards.encodePath(folder);
                const displayName = folder.split("/").pop();
                let title = displayName;
                let subtitle = "";
                try {
                    const resp = await ContentBase.asyncFetch(`${encoded}/_index.json`);
                    if (resp.ok) {
                        const meta = await resp.json();
                        title = meta.title || displayName;
                        subtitle = ContentCards.cardSubtitle(meta);
                    } else {
                        const mdResp = await ContentBase.asyncFetch(`${encoded}/_index.md`);
                        if (mdResp.ok) {
                            const meta = parseMarkdown(await mdResp.text());
                            title = meta.title || displayName;
                            subtitle = ContentCards.cardSubtitle(meta);
                        }
                    }
                } catch (e) {
                    /* fall back to folder name */
                }
                let href = `${encoded}/index.html`;
                // If folder has path separators, add back link to current page
                if (folder.includes("/")) {
                    const depth = folder.split("/").length;
                    const backPath = "../".repeat(depth) + "index.html";
                    href += `?back=${encodeURIComponent(backPath)}`;
                }
                return {
                    folder,
                    title,
                    subtitle,
                    href,
                    arrow: "View",
                    link: true,
                };
            }

            const resolved = { ...item };
            resolved.title = item.title || item.folder || "";
            resolved.subtitle = item.subtitle || "";
            if (item.link === false) {
                resolved.link = false;
            } else if (item.href) {
                resolved.link = true;
            } else if (item.folder) {
                resolved.href = `${ContentCards.encodePath(item.folder)}/index.html`;
                resolved.link = true;
            } else {
                resolved.link = false;
            }
            resolved.arrow = item.arrow || "View";
            return resolved;
        });

        this.resolvedItems = await Promise.all(promises);
        await super.resolve();
    }

    computeHtml () {
        const title = this.json.title || "";
        const items = this.resolvedItems;
        const columns = this.json.columns || Math.min(items.length, 3);
        const colsClass = columns === 2 ? " cols-2" : "";

        let html = "";
        if (title) html += `<h2>${title}</h2>`;
        html += `<div class="card-grid${colsClass}">`;

        for (let idx = 0; idx < items.length; idx++) {
            const item = items[idx];
            const tag = item.link ? "a" : "div";
            const hrefAttr = item.link ? ` href="${item.href}"` : "";

            // Last item spans remaining columns if row is partial
            const isLast = idx === items.length - 1;
            const remainder = items.length % columns;
            const spanClass = (isLast && remainder > 0) ? ` card-span-${columns - remainder + 1}` : "";

            html += `<${tag} class="card${spanClass}"${hrefAttr}>`;
            html += `<h3>${item.title}</h3>`;
            if (item.date) {
                html += `<div class="card-date">${item.date}</div>`;
            }
            if (item.subtitle) {
                html += `<p>${item.subtitle}</p>`;
            }
            if (item.link) {
                html += `<span class="arrow">${item.arrow} &rarr;</span>`;
            }
            html += `</${tag}>`;
        }

        html += "</div>";
        return html;
    }
}
