document.querySelectorAll("method").forEach(m => m.classList.add("collapsed"));

document.querySelectorAll(".collapsible").forEach(el => {
    el.addEventListener("click", () => {
        const content = el.nextElementSibling;
        if (content) content.classList.toggle("show");
        el.classList.toggle("expanded");
        const m = el.closest("method");
        if (m) {
            m.classList.toggle("expanded");
            m.classList.toggle("collapsed");
        }
    });
});
