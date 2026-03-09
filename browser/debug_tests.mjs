import { createServer } from "http";
import { readFileSync, existsSync } from "fs";
import { join, extname } from "path";
import { chromium } from "playwright";

const BROWSER_DIR = "browser";
const PORT = 8789;

const MIME = {".html":"text/html",".js":"application/javascript",".wasm":"application/wasm"};
const server = createServer((req, res) => {
  const filePath = join(BROWSER_DIR, req.url === "/" ? "index.html" : req.url);
  if (!existsSync(filePath)) { res.writeHead(404); res.end("Not found"); return; }
  const ext = extname(filePath);
  res.writeHead(200, {"Content-Type": MIME[ext] || "application/octet-stream"});
  res.end(readFileSync(filePath));
});

await new Promise(r => server.listen(PORT, r));
const browser = await chromium.launch();
try {
  const page = await browser.newPage();
  const pageErrors = [];
  page.on("console", msg => {
    if (msg.type() === "error") console.log("CONSOLE ERROR:", msg.text());
  });
  page.on("pageerror", err => {
    pageErrors.push(err.message);
    console.error("PAGE_ERROR:", err.message);
  });

  await page.goto("http://localhost:" + PORT + "/test.html");
  await new Promise(r => setTimeout(r, 8000));

  const results = await page.evaluate(() => {
    return Array.from(document.querySelectorAll(".test")).map(e => {
      const passed = !!e.querySelector(".pass");
      const name = e.querySelector(".label")?.textContent || "";
      const detail = e.querySelector(".detail")?.textContent || "";
      return { passed, name, detail };
    });
  });

  let passed = 0;
  for (const t of results) {
    if (t.passed) {
      passed++;
      console.log("\x1b[32mPASS\x1b[0m", t.name);
    } else {
      console.log("\x1b[31mFAIL\x1b[0m", t.name);
      if (t.detail) console.log("     " + t.detail.replace(/\n/g, "\n     "));
    }
  }
  console.log(`\n${passed} / ${results.length} passed`);
  if (pageErrors.length) console.log("Page errors:", pageErrors);
} catch(e) { console.error("ERROR:", e.message); }
await browser.close();
server.close();
