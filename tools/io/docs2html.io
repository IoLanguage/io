#!/usr/local/bin/io

// Generate Io documentation pages in the colvmn + ContentClassDoc style.
//
// Writes:
//   $docsPath/_index.md               markdown landing (baked to HTML by static-gen.js)
//   $docsPath/index.html              colvmn shell
//   $docsPath/<Proto>/_index.json     ContentClassDoc JSON per proto
//   $docsPath/<Proto>/index.html      colvmn shell
//
// Args:
//   1: output directory (e.g., docs/Reference or docs/Technical Notes/Implementation Reference/C Implementation Reference)
//   2: source kind — "docs" (Io-visible, default) or "cdocs" (C internals)

docsPath := System args at(1)
sourceKind := System args at(2) ifNilEval("docs")

isCdocs := sourceKind == "cdocs"
pageTitle := "Reference"
pageSubtitle := if(isCdocs,
    "C-internal implementation details extracted from `cdoc` and `cmetadoc` comments.",
    "Browse every built-in object and method in Io's standard library.")
sectionName := if(isCdocs, "Functions", "Slots")

// Extract docs.txt / cdocs.txt in-process (works under wasmtime).
doRelativeFile("DocsExtractor.io")
DocsExtractor clone setPath("libs/iovm") extract

// ----- parse -----

prototypes := Map clone
txtFile := File with(Path with("libs/iovm/docs", sourceKind .. ".txt"))

if(txtFile exists not,
    ("docs2html: " .. txtFile path .. " not found; nothing to do.") println
    System exit(0)
)

txtFile contents split("------\n") foreach(e,
    if(e strip size > 0,
        isSlot := e beginsWithSeq("doc") or e beginsWithSeq("cdoc")
        h := e beforeSeq("\n") afterSeq(" ")
        if(h,
            h = h asMutable strip asSymbol
            protoName := h beforeSeq(" ") ?asMutable ?strip ?asSymbol
            slotName := h afterSeq(" ") ?asMutable ?strip ?asSymbol
            description := e afterSeq("\n")

            if(protoName and slotName,
                p := prototypes atIfAbsentPut(protoName, Map clone atPut("slots", Map clone))
                if(isSlot,
                    p at("slots") atPut(slotName, description)
                ,
                    p atPut(slotName, description)
                )
            )
        )
    )
)

// ----- filter VM-internal structures -----
// Drop protos with neither description nor slots (e.g., Tag, Token —
// C data structures that happen to use metadoc for copyright only).

prototypes keys foreach(name,
    p := prototypes at(name)
    hasDesc := p at("description") != nil and p at("description") size > 0
    hasSlots := p at("slots") size > 0
    if(hasDesc not and hasSlots not, prototypes removeAt(name))
)

// ----- categorize for landing -----

defaultCat := if(isCdocs, "Core", "Uncategorized")
byCategory := Map clone
prototypes foreach(name, p,
    cat := p at("category") ifNilEval(defaultCat)
    cat = cat asMutable strip
    if(cat size == 0, cat = defaultCat)
    byCategory atIfAbsentPut(cat, List clone) append(name)
)

catOrder := byCategory keys sort
if(catOrder contains("Core"),
    catOrder remove("Core")
    catOrder prepend("Core")
)
if(catOrder contains("Uncategorized"),
    catOrder remove("Uncategorized")
    catOrder append("Uncategorized")
)

// ----- write _index.md landing -----

Directory with(docsPath) createIfAbsent

md := Sequence clone
md appendSeq("# ", pageTitle, "\n\n")
md appendSeq(pageSubtitle, "\n\n")
md appendSeq("## Objects\n\n")
catOrder foreach(cat,
    md appendSeq("- ", cat, "\n")
    byCategory at(cat) sort foreach(proto,
        name := proto asString
        md appendSeq("  - [", name, "](", name, "/index.html)\n")
    )
)
md appendSeq("\n")

File with(Path with(docsPath, "_index.md")) remove open write(md) close

// ----- compute depth-relative prefixes -----

pathSegments := docsPath split("/") select(size > 0)
depth := pathSegments size
shellUp := Sequence clone
depth repeat(shellUp appendSeq("../"))

protoUp := Sequence clone
(depth + 1) repeat(protoUp appendSeq("../"))

// ----- write colvmn shell for a given dir -----

writeShell := method(dir, cssUp, title,
    s := Sequence clone
    s appendSeq("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n")
    s appendSeq("<meta charset=\"UTF-8\">\n")
    s appendSeq("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n")
    s appendSeq("<link rel=\"stylesheet\" href=\"", cssUp, "colvmn/style.css\">\n")
    s appendSeq("<title>", title, " &ndash; Io</title>\n")
    s appendSeq("<link rel=\"alternate\" type=\"text/plain\" title=\"llms.txt\" href=\"/llms.txt\">\n")
    s appendSeq("</head>\n<body>\n")
    s appendSeq("<div class=\"page\"></div>\n")
    s appendSeq("<script src=\"", cssUp, "colvmn/layout/layout.js\" type=\"module\"></script>\n")
    s appendSeq("</body>\n</html>\n")
    File with(Path with(dir, "index.html")) remove open write(s) close
)

writeShell(docsPath, shellUp, pageTitle)

// ----- write per-proto _index.json + shell -----

// JSON string escaper — handles the minimum needed for doc bodies.
jsonEscape := method(s,
    s asMutable \
        replaceSeq("\\", "\\\\") \
        replaceSeq("\"", "\\\"") \
        replaceSeq("\n", "\\n") \
        replaceSeq("\r", "\\r") \
        replaceSeq("\t", "\\t")
)

// The extractor stores slotName as "name(sig)" for methods with args,
// or just "name" otherwise. Split on the first '(' to separate them.
splitNameSig := method(slotKey,
    s := slotKey asString
    if(s containsSeq("("),
        list(s beforeSeq("("), "(" .. s afterSeq("("))
    ,
        list(s, nil)
    )
)

prototypes foreach(protoName, p,
    name := protoName asString
    slots := p at("slots")
    desc := p at("description") ifNilEval("")
    if(desc size > 0, desc = desc asMutable strip)

    // Build JSON
    j := Sequence clone
    j appendSeq("{\n")
    j appendSeq("  \"title\": \"", jsonEscape(name), "\",\n")
    if(desc size > 0,
        j appendSeq("  \"subtitle\": \"", jsonEscape(desc), "\",\n")
    )
    j appendSeq("  \"content\": [\n")
    j appendSeq("    {\n")
    j appendSeq("      \"type\": \"ContentClassDoc\",\n")
    j appendSeq("      \"sections\": [\n")
    j appendSeq("        {\n")
    j appendSeq("          \"name\": \"", jsonEscape(sectionName), "\",\n")
    j appendSeq("          \"categories\": [\n")
    j appendSeq("            {\n")
    j appendSeq("              \"members\": [\n")

    slotNames := if(slots, slots keys sort, list())
    // Re-sort by the bare name (without signature) so "at" and "at(i)"
    // are alphabetized on the name, not the paren.
    slotNames = slotNames sortBy(block(a, b,
        splitNameSig(a) at(0) < splitNameSig(b) at(0)
    ))
    slotNames foreach(i, slotName,
        body := slots at(slotName) ifNilEval("")
        if(body size > 0, body = body asMutable strip)
        parts := splitNameSig(slotName)
        bareName := parts at(0)
        sig := parts at(1)

        j appendSeq("                {")
        j appendSeq("\"name\": \"", jsonEscape(bareName), "\"")
        if(sig, j appendSeq(", \"signature\": \"", jsonEscape(sig), "\""))
        if(body size > 0,
            j appendSeq(", \"description\": \"", jsonEscape(body), "\"")
        )
        j appendSeq("}")
        if(i < (slotNames size - 1), j appendSeq(","))
        j appendSeq("\n")
    )

    j appendSeq("              ]\n")
    j appendSeq("            }\n")
    j appendSeq("          ]\n")
    j appendSeq("        }\n")
    j appendSeq("      ]\n")
    j appendSeq("    }\n")
    j appendSeq("  ]\n")
    j appendSeq("}\n")

    Directory with(Path with(docsPath, name)) createIfAbsent
    File with(Path with(docsPath, name, "_index.json")) remove open write(j) close
    writeShell(Path with(docsPath, name), protoUp, name)
)

("Wrote " .. prototypes size .. " " .. pageTitle .. " protos to " .. docsPath) println
