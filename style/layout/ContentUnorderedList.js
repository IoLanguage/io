import { ContentBase, renderList } from "./ContentBase.js";

export class ContentUnorderedList extends ContentBase {
    computeHtml () {
        return renderList(this.json, "ul", this.depth);
    }
}
