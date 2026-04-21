export function renderList (json, tag, depth) {
    const title = json.title || "";
    const items = json.items || [];

    function renderItems (itemList, listTag) {
        let html = `<${listTag}>`;
        for (const item of itemList) {
            if (typeof item === "string") {
                html += `<li>${item}</li>`;
            } else {
                html += `<li>${item.text || ""}`;
                if (Array.isArray(item.content)) {
                    for (const child of item.content) {
                        const node = ContentBase.fromJson(child, depth + 1);
                        html += node.computeHtml();
                    }
                }
                html += "</li>";
            }
        }
        html += `</${listTag}>`;
        return html;
    }

    const listHtml = renderItems(items, tag);

    if (depth >= 1) {
        let html = '<div class="section">';
        if (title) html += `<div class="section-head">${title}</div>`;
        html += `<div class="section-body">${listHtml}</div>`;
        html += "</div>";
        return html;
    }

    let html = "";
    if (title) html += `<h2>${title}</h2>`;
    html += `<div class="section"><div class="section-body">${listHtml}</div></div>`;
    return html;
}


export class ContentBase {
    static typeMap = {};

    static _fetchFn = null;

    static setFetchFn (fn) {
        ContentBase._fetchFn = fn;
    }

    static asyncFetch (url) {
        if (ContentBase._fetchFn) {
            return ContentBase._fetchFn(url);
        }
        return fetch(url, { cache: "no-store" });
    }

    constructor () {
        this.json = null;
        this.children = [];
        this.depth = 0;
    }

    setJson (json, depth = 0) {
        this.json = json;
        this.depth = depth;
        if (Array.isArray(json.content)) {
            this.children = json.content.map(c => ContentBase.fromJson(c, depth + 1));
        }
    }

    async resolve () {
        for (const child of this.children) {
            await child.resolve();
        }
    }

    computeHtml () {
        return this.children.map(c => c.computeHtml()).join("");
    }

    postRender (page) {
        this.children.forEach(c => c.postRender(page));
    }

    static fromJson (json, depth = 0) {
        const Cls = ContentBase.typeMap[json.type];
        if (!Cls) {
            console.warn(`Unknown content type: ${json.type}`);
            return new ContentBase();
        }
        const instance = new Cls();
        instance.setJson(json, depth);
        return instance;
    }
}
