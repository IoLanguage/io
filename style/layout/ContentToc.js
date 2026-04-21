import { ContentBase } from "./ContentBase.js";
import { slugify } from "./MarkdownParser.js";

export class ContentToc extends ContentBase {
    computeHtml () {
        const items = this.json.items || [];
        let html = '<nav class="toc"><ul>';
        for (const item of items) {
            const slug = slugify(item);
            html += `<li><a href="#${slug}">${item}</a></li>`;
        }
        html += "</ul></nav>";
        return html;
    }
}
