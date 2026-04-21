import { ContentBase } from "./ContentBase.js";
import { slugify } from "./MarkdownParser.js";

export class ContentText extends ContentBase {
    computeHtml () {
        const title = this.json.title || "";
        const body = this.json.body || "";
        const layout = this.json.layout;
        const childHtml = super.computeHtml();

        if (layout === "2 column") {
            let html = '<div class="section-two-col">';
            html += `<div class="section-label">${title}</div>`;
            html += '<div class="section-body">';
            if (body) html += body;
            html += "</div></div>";
            if (childHtml) html += childHtml;
            return html;
        }

        if (this.depth === 0) {
            let html = "";
            if (title) html += `<h2 id="${slugify(title)}">${title}</h2>`;
            if (body) {
                html += '<div class="section"><div class="section-body">';
                html += body;
                html += "</div></div>";
            }
            if (childHtml) html += childHtml;
            return html;
        }

        let html = '<div class="section">';
        if (title) html += `<div class="section-head">${title}</div>`;
        html += '<div class="section-body">';
        if (body) html += body;
        html += "</div></div>";
        if (childHtml) html += childHtml;
        return html;
    }
}
