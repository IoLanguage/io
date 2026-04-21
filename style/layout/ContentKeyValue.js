import { ContentBase } from "./ContentBase.js";

export class ContentKeyValue extends ContentBase {
    computeHtml () {
        const title = this.json.title || "";
        const items = this.json.items || {};
        const childHtml = super.computeHtml();

        let innerHtml = "";
        for (const [key, value] of Object.entries(items)) {
            innerHtml += `<p><strong>${key}:</strong> ${value}</p>`;
        }

        if (this.depth >= 1) {
            let html = '<div class="section">';
            if (title) html += `<div class="section-head">${title}</div>`;
            html += `<div class="section-body">${innerHtml}</div>`;
            html += "</div>";
            return html + childHtml;
        }

        let html = "";
        if (title) html += `<h2>${title}</h2>`;
        html += `<div class="section"><div class="section-body">${innerHtml}</div></div>`;
        return html + childHtml;
    }
}
