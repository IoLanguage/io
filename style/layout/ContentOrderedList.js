import { ContentBase, renderList } from "./ContentBase.js";

export class ContentOrderedList extends ContentBase {
    computeHtml () {
        return renderList(this.json, "ol", this.depth);
    }
}
