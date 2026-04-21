import { ContentBase } from "./ContentBase.js";
import { slugify } from "./MarkdownParser.js";

const MONTH_NAMES = ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];

export class ContentTimeline extends ContentBase {
    computeHtml () {
        const items = this.json.items || [];
        if (!items.length) return "";
        const title = this.json.title || "";

        // Group events by month into columns
        const columns = [];
        let currentKey = null;
        for (const item of items) {
            const [year, month] = item.date.split("-").map(Number);
            const key = item.date;
            const ev = {
                title: item.title,
                subtitle: item.subtitle || "",
                href: item.href || ""
            };
            if (key !== currentKey) {
                columns.push({
                    year,
                    month,
                    monthIndex: year * 12 + month,
                    events: [ev]
                });
                currentKey = key;
            } else {
                columns[columns.length - 1].events.push(ev);
            }
        }

        // Axis year labels
        const startYear = columns[0].year;
        const endYear = columns[columns.length - 1].year;

        // Build column layout data
        let prevYear = null;
        let prevMonthIndex = null;
        let nextAbove = true;

        const colData = columns.map((col, i) => {
            // Date label
            const monthName = MONTH_NAMES[col.month - 1];
            const dateLabel = (prevYear !== col.year)
                ? `${monthName} ${col.year}`
                : monthName;
            prevYear = col.year;

            // Proportional spacing: linear in months, with a floor so adjacent
            // events stay readable and a soft ceiling so multi-decade gaps
            // don't produce excessive blank track.
            const PX_PER_MONTH = 8;
            const MIN_MARGIN = 60;
            const MAX_MARGIN = 1400;
            let margin;
            if (i === 0) {
                margin = 100;
            } else {
                const diff = col.monthIndex - prevMonthIndex;
                margin = Math.max(MIN_MARGIN, Math.min(diff * PX_PER_MONTH, MAX_MARGIN));
            }
            prevMonthIndex = col.monthIndex;

            // Above/below placement
            let aboveEvents, belowEvents;
            if (col.events.length > 1) {
                aboveEvents = [col.events[0]];
                belowEvents = col.events.slice(1);
                nextAbove = true;
            } else {
                if (nextAbove) {
                    aboveEvents = col.events;
                    belowEvents = [];
                } else {
                    aboveEvents = [];
                    belowEvents = col.events;
                }
                nextAbove = !nextAbove;
            }

            return { dateLabel, margin, aboveEvents, belowEvents };
        });

        let html = "";
        if (title) html += `<h2 id="${slugify(title)}">${title}</h2>`;

        // Toolbar (event listeners attached in postRender)
        html += '<div class="timeline-toolbar">';
        html += '<button class="zoom-btn zoom-btn-minus">&minus;</button>';
        html += '<button class="zoom-btn zoom-btn-plus">&plus;</button>';
        html += '<button class="details-toggle">Hide Details</button>';
        html += "</div>";

        // Render timeline track
        html += '<div class="timeline-wrap"><div class="timeline-track">';

        html += `<div class="axis-year axis-year-start">${startYear}</div>`;
        if (endYear !== startYear) {
            html += `<div class="axis-year axis-year-end">${endYear}</div>`;
        }

        const renderTitle = (ev) => ev.href
            ? `<div class="event-title"><a class="event-link" href="${ev.href}">${ev.title}<span class="event-link-arrow">&rarr;</span></a></div>`
            : `<div class="event-title">${ev.title}</div>`;

        for (const col of colData) {
            html += `<div class="tl-col" style="margin-left: ${col.margin}px;">`;

            // Above half
            html += '<div class="tl-above">';
            if (col.aboveEvents.length > 0) {
                for (let j = 0; j < col.aboveEvents.length; j++) {
                    const ev = col.aboveEvents[j];
                    html += '<div class="event">';
                    if (j === 0) html += `<div class="event-date">${col.dateLabel}</div>`;
                    html += renderTitle(ev);
                    if (ev.subtitle) html += `<div class="event-desc">${ev.subtitle}</div>`;
                    html += "</div>";
                }
                html += '<div class="tl-stem"></div>';
            }
            html += "</div>";

            // Below half
            html += '<div class="tl-below">';
            if (col.belowEvents.length > 0) {
                html += '<div class="tl-stem"></div>';
                for (let j = 0; j < col.belowEvents.length; j++) {
                    const ev = col.belowEvents[j];
                    html += '<div class="event">';
                    if (j === 0 && col.aboveEvents.length === 0) {
                        html += `<div class="event-date">${col.dateLabel}</div>`;
                    }
                    html += renderTitle(ev);
                    if (ev.subtitle) html += `<div class="event-desc">${ev.subtitle}</div>`;
                    html += "</div>";
                }
            }
            html += "</div>";

            html += "</div>";
        }

        // End spacer
        html += '<div style="width: 120px; flex-shrink: 0;"></div>';
        html += "</div></div>";

        // Footer
        html += '<div class="timeline-footer">';
        html += '<hr class="timeline-footer-rule">';
        html += '<div class="timeline-footer-row">';
        html += '<span class="scroll-hint">Scroll horizontally to explore &rarr;</span>';
        html += "</div></div>";

        return html;
    }

    postRender (page) {
        const wrap = page.querySelector(".timeline-wrap");
        const track = page.querySelector(".timeline-track");
        if (!wrap || !track) return;

        const minusBtn = page.querySelector(".zoom-btn-minus");
        const plusBtn = page.querySelector(".zoom-btn-plus");
        const detailsBtn = page.querySelector(".details-toggle");

        // Capture natural width at zoom=1
        const naturalWidth = track.scrollWidth;
        let userWantsDetails = true;

        function getMinZoom () {
            return Math.max(wrap.clientWidth / naturalWidth, 0.3);
        }

        function applyZoom (newZoom) {
            const minZoom = getMinZoom();
            newZoom = Math.min(Math.max(newZoom, minZoom), 1.8);
            track.style.zoom = newZoom.toFixed(2);

            // Auto-hide details at or near min zoom
            if (newZoom <= minZoom + 0.01) {
                page.classList.add("hide-details");
                detailsBtn.textContent = "Show Details";
            } else if (userWantsDetails) {
                page.classList.remove("hide-details");
                detailsBtn.textContent = "Hide Details";
            }
        }

        minusBtn.addEventListener("click", () => {
            const cur = parseFloat(track.style.zoom || 1);
            applyZoom(cur - 0.15);
        });

        plusBtn.addEventListener("click", () => {
            const cur = parseFloat(track.style.zoom || 1);
            applyZoom(cur + 0.15);
        });

        // Click-drag to scroll with momentum
        let isDragging = false;
        let dragStartX = 0;
        let scrollStartX = 0;
        let lastClientX = 0;
        let lastMoveTime = 0;
        let velocity = 0;
        let momentumId = null;

        wrap.style.cursor = "grab";

        wrap.addEventListener("mousedown", (e) => {
            isDragging = true;
            dragStartX = e.clientX;
            lastClientX = e.clientX;
            scrollStartX = wrap.scrollLeft;
            lastMoveTime = performance.now();
            velocity = 0;
            if (momentumId) {
                cancelAnimationFrame(momentumId);
                momentumId = null;
            }
            wrap.style.cursor = "grabbing";
            wrap.style.userSelect = "none";
            e.preventDefault();
        });

        window.addEventListener("mousemove", (e) => {
            if (!isDragging) return;
            const now = performance.now();
            const dt = now - lastMoveTime;
            const dx = e.clientX - lastClientX;
            if (dt > 0) {
                velocity = dx / dt;
            }
            lastClientX = e.clientX;
            lastMoveTime = now;
            wrap.scrollLeft = scrollStartX - (e.clientX - dragStartX);
        });

        window.addEventListener("mouseup", () => {
            if (!isDragging) return;
            isDragging = false;
            wrap.style.cursor = "grab";
            wrap.style.userSelect = "";

            // Apply momentum
            const friction = 0.95;
            let v = -velocity * 16; // scale to px/frame

            function step () {
                if (Math.abs(v) < 0.5) {
                    momentumId = null;
                    return;
                }
                wrap.scrollLeft += v;
                v *= friction;
                momentumId = requestAnimationFrame(step);
            }
            if (Math.abs(v) > 0.5) {
                momentumId = requestAnimationFrame(step);
            }
        });

        // Auto-scroll to the end of the timeline
        wrap.scrollLeft = wrap.scrollWidth;

        detailsBtn.addEventListener("click", () => {
            const cur = parseFloat(track.style.zoom || 1);
            const minZoom = getMinZoom();
            // At min zoom, details always stay hidden
            if (cur <= minZoom + 0.01) return;

            userWantsDetails = !userWantsDetails;
            page.classList.toggle("hide-details");
            detailsBtn.textContent = page.classList.contains("hide-details")
                ? "Show Details" : "Hide Details";
        });
    }
}
