import { ContentBase } from "./ContentBase.js";

const TH_STYLE = "text-align: left; font-size: 12px; font-weight: 600; padding: 12px 16px 12px 0; border-top: 1px solid #ccc; border-bottom: 1px solid #ccc; letter-spacing: 0.3px; color: #555;";
const TD_STYLE = "padding: 10px 16px 10px 0; border-bottom: 1px solid #eee; color: #555; vertical-align: top;";
const TD_LAST_STYLE = "padding: 10px 16px 10px 0; border-bottom: 1px solid #ccc; color: #555; vertical-align: top;";

function renderCell (value) {
    if (Array.isArray(value)) {
        const items = value.map(v => `<li>${v}</li>`).join("");
        return `<ul style="margin:0; padding-left:18px;">${items}</ul>`;
    }
    return value || "";
}

export class ContentTable extends ContentBase {
    computeHtml () {
        const title = this.json.title || "";
        const subtitle = this.json.subtitle || "";
        const columns = this.json.columns || [];
        const rows = this.json.rows || [];
        const note = this.json.note || "";
        const childHtml = super.computeHtml();

        // Header row
        let thead = "<thead><tr>";
        for (const col of columns) {
            thead += `<th style="${TH_STYLE}">${col}</th>`;
        }
        thead += "</tr></thead>";

        // Body rows
        let tbody = "<tbody>";
        for (let i = 0; i < rows.length; i++) {
            const row = rows[i];
            const isLast = i === rows.length - 1;
            const style = isLast ? TD_LAST_STYLE : TD_STYLE;
            tbody += "<tr>";
            for (const cell of row) {
                tbody += `<td style="${style}">${renderCell(cell)}</td>`;
            }
            tbody += "</tr>";
        }
        tbody += "</tbody>";

        const table = `<table style="width: 100%; border-collapse: collapse; font-size: 13px;">${thead}${tbody}</table>`;

        let noteHtml = "";
        if (note) {
            noteHtml = `<p style="font-size: 12px; font-style: italic; color: #888; margin-top: 12px;">${note}</p>`;
        }

        let innerHtml = "";
        if (subtitle) innerHtml += `<p>${subtitle}</p>`;
        innerHtml += table + noteHtml;

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
