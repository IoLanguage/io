import { ContentBase } from "./ContentBase.js";
import { ContentText } from "./ContentText.js";
import { ContentCards } from "./ContentCards.js";
import { ContentKeyValue } from "./ContentKeyValue.js";
import { ContentUnorderedList } from "./ContentUnorderedList.js";
import { ContentOrderedList } from "./ContentOrderedList.js";
import { ContentTable } from "./ContentTable.js";
import { ContentImage } from "./ContentImage.js";
import { ContentTimeline } from "./ContentTimeline.js";
import { ContentToc } from "./ContentToc.js";
import { PageIndex } from "./PageIndex.js";

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
};

document.addEventListener("DOMContentLoaded", () => {
    new PageIndex().init();
});
