#!/usr/local/bin/io

docsPath := System args at(1)

// Inline doc extraction (no shelling out — works on WASM)
doRelativeFile("DocsExtractor.io")
DocsExtractor clone setPath("libs/iovm") extract

prototypes := Map clone
modules := Map clone


File _write := File getSlot("write")
File writeln := method(call evalArgs foreach(s, self _write(s)); self _write("\n"); self)

readFolder := method(path,
	file := File with(Path with(path, "/docs/docs.txt"))
	if(file exists, nil,
	    "readFolder(#{path}/docs/docs.txt fails\n" interpolate println
 	    return)
        file contents split("------\n") foreach(e,
		isSlot := e beginsWithSeq("doc")
		h := e beforeSeq("\n") afterSeq(" ") 
		if(h,
			h = h asMutable strip asSymbol
			protoName := h beforeSeq(" ") ?asMutable ?strip ?asSymbol
			slotName := h afterSeq(" ") ?asMutable ?strip ?asSymbol
			description := e afterSeq("\n")
			p := prototypes atIfAbsentPut(protoName, Map clone atPut("slots", Map clone))
		
			moduleName := path lastPathComponent
			if(moduleName == "iovm", moduleName = "Core")
			p atPut("module", moduleName)
			m := modules atIfAbsentPut(moduleName, Map clone)
			modules atPut(moduleName, m)
			m atPut(protoName, p)
		
			if(protoName == nil or slotName == nil, writeln("ERROR: " .. e))
			if(isSlot, 
				p at("slots") atPut(slotName, description)
			,
				p atPut(slotName, description)
			)
		,
			if(protoName == nil or slotName == nil, writeln("ERROR: " .. e))
		)
		
	)
)

readFolder("libs/iovm")

// ------------------------------

/*
ReferenceDoc := Object clone do(
	modules :: = List clone
	print := method(
		modules foreach(printIndex)
		modules foreach(print)
	)
)

ModuleDoc := Object clone do(
	refProtos :: = Map clone
	path ::= nil
	
	refProtoNamed := method(name,
		refProtos atIfAbsentPut(name, ProtoDoc clone setName(name))
	)
	
	read := method(
		File with(Path with(path, "/docs/docs.txt")) contents split("------\n") foreach(e,
			isSlot := e beginsWithSeq("doc")
			h := e beforeSeq("\n") afterSeq(" ") 
			if(h,
				h = h asMutable strip asSymbol
				protoName := h beforeSeq(" ") ?asMutable ?strip ?asSymbol
				slotName  := h afterSeq(" ") ?asMutable ?strip ?asSymbol
				description := e afterSeq("\n")
				
				if(isSlot,
					refProtoNamed(protoName) addSlot(slotName, description)
				,
					refProtoNamed(protoName) addMetaSlot(slotName, description)
				)
		)
	)

	print := method(
		refProtos foreach(printIndex)
		refProtos foreach(print)
	)
)

ProtoDoc := Object clone do(
	init := method(self slots := List clone)
	printIndex := method(
		nil
	)
	print := method(
		printIndex
		slots foreach(print)
	)	
)

SlotDoc := Object clone do(
	name ::= nil
	value ::= nil
	
	printIndex := method(
		nil
	)
	print := method(
		nil
	)
)

*/

protoNames := prototypes keys sort
moduleNames := modules keys sort 

moduleNames remove("Core") prepend("Core")

categories := Map clone
modules foreach(moduleName, module,
	firstProto := nil
	firstProtoName := module keys sort detect(k, module at(k) at("category"))
	
	catNameMap := Map clone
	module values select(at("category")) foreach(m, 
		count := catNameMap at(m at("category")) 
		if(count == nil, count = 0)
		catNameMap atPut(m at("category"), count + 1)
	)
	
	maxCount := 0
	catName := nil
	catNameMap foreach(name, count,
		if(count > maxCount, catName = name; maxCount = count)
	)
	
	module foreach(k, v, if(v at("category") != catName, module removeAt(k)))
	
	/*	
	if(firstProtoName, firstProto := module at(firstProtoName))
	if(firstProto,
		catName := firstProto at("category")
	, 
		writeln("warning: no cat for ", moduleName, " ", module keys)
		catName := "Other"
	)
	*/
	
	if(catName == nil, catName = "Misc")
	cat := categories atIfAbsentPut(catName asMutable strip, Map clone)
	cat atPut(moduleName, module)
/*
	catName isNil ifFalse(
	  cat := categories atIfAbsentPut(catName asMutable strip, Map clone)
	  cat atPut(moduleName, module)
	)
*/
)

Section := Object clone do(
	items ::= nil
	name ::= ""
	path ::= ""
	
	sortedItems := method(
		items keys sort map(k, itemNamed(k))
	)
	
	linksHtml := method(selectedName,
		s := Sequence clone
		sortedItems foreach(item,
			class := if(item name == selectedName, "indexItemSelected", "indexItem")
			url := Path with(path, item name, "index.html")
			if(item items size == 1,
				url := Path with(path, item name, item items keys first, "index.html")
			)
			s appendSeq("<div class=", class, "><a href=", url, ">", item name, "</a></div>\n")
			s
		)
	)
	
	itemNamed := method(name,
		Section clone setItems(items at(name)) setName(name)
	)
)

Categories := Section clone setItems(categories) 
// JSON DOCS --------------------------------------------------------

jsonFile := File with(Path with(docsPath, "docs.json")) remove open
jsonFile writeln(Categories items asJson)
jsonFile close

// TEMPLATES --------------------------------------------------------

indexTemplate := File with("docs/templates/index.html.template") contents
protoTemplate := File with("docs/templates/proto.html.template") contents

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)

columnTd := method(content,
	"<td valign=\"top\" class=\"column\">\n" .. content .. "</td>\n"
)

renderIndex := method(title, cssPath, columns,
	indexTemplate asMutable interpolateInPlace
)

descriptionHtml := method(p,
	s := Sequence clone
	if(p at("description"),
		s appendSeq("<tr>\n<td align=right></td>\n<td></td>\n<td>" .. p at("description") .. "</td></tr>\n")
	)
	s appendSeq("<tr><td colspan=3>&nbsp;</td></tr>\n")
	s appendSeq("<tr><td colspan=3>&nbsp;</td></tr>\n")
	s
)

slotsHtml := method(protoName, slots,
	s := Sequence clone
	if(slots,
		s appendSeq("<tr><td colspan=3>&nbsp;</td></tr>\n")
		s appendSeq("<tr>\n<td align=right>\n</td>\n<td></td>\n<td>\n")
		if(slots size > 0, s appendSeq("<hr align=left color=#ddd height=1>\n"))
		s appendSeq("<br><br>\n")

		slots keys sort foreach(k,
			desc := slots at(k)
			if(desc, desc = desc strip)
			isPrivate := desc ?beginsWithSeq("Private")
			isDeprecated := desc ?beginsWithSeq("Deprecated")
			if(isPrivate, s appendSeq("<font color=#888>\n"))
			if(isDeprecated, s appendSeq("<font color=#55a>\n"))
			s appendSeq("<a name=\"" .. protoName .. "-" .. k beforeSeq("(") asHtml .. "\"></a><b>\n")
			s appendSeq(k asHtml .. "\n")
			s appendSeq("</b>\n<p>\n<div class=slotDescription>\n")
			if(desc, s appendSeq(desc .. "\n"), s appendSeq("<div class=error>undocumented</div>\n"))
			if(isPrivate or isDeprecated, s appendSeq("</font>\n"))
			s appendSeq("</div>\n")
		)
	)
	s appendSeq("</td>\n</tr>\n")
	s
)

// CATEGORIES -------------------------------------------------------

columns := "<tr>\n" .. columnTd(Categories linksHtml) .. "</tr>"
outFile := File with(Path with(docsPath, "index.html")) remove open
outFile write(renderIndex("Io Reference", "../docs.css", columns))
outFile close

// CATEGORY -------------------------------------------------------

Categories sortedItems foreach(cat,
	cat sortedItems foreach(addon,
		columns := "<tr>\n" .. columnTd(Categories setPath("..") linksHtml(cat name)) .. columnTd(cat linksHtml) .. "</tr>"
		outFile := Directory with(Path with(docsPath, cat name)) createIfAbsent fileNamed("index.html") remove open
		outFile write(renderIndex("Io Reference", "../../docs.css", columns))
		outFile close
	)
)

// ADDON -------------------------------------------------------

Categories sortedItems foreach(cat,
	cat sortedItems foreach(addon,
		addon sortedItems foreach(proto,
			columns := "<tr>\n" .. columnTd(Categories setPath("../..") linksHtml(cat name)) .. columnTd(cat setPath("..") linksHtml(addon name)) .. columnTd(addon linksHtml) .. "</tr>"
			outFile := Directory with(Path with(docsPath, cat name, addon name)) createIfAbsent fileNamed("index.html") remove open
			outFile write(renderIndex("Io Reference", "../../../docs.css", columns))
			outFile close
		)
	)
)

// PROTOS -------------------------------------------------------

Categories sortedItems foreach(cat,
	cat sortedItems foreach(addon,
		addon items foreach(protoName, p,
			moduleName = p at("module")
			outFile := Directory with(Path with(docsPath, cat name, moduleName, protoName)) createIfAbsent fileNamed("index.html") remove open

			breadcrumbs := Sequence clone
			breadcrumbs appendSeq("<a class='column' href='../../index.html'>")
			breadcrumbs appendSeq(cat name)
			breadcrumbs appendSeq("</a>\n&nbsp;&nbsp;<font color=#ccc>/</font>&nbsp;&nbsp;\n")
			breadcrumbs appendSeq("<a class='column' href='../index.html'>")
			breadcrumbs appendSeq(moduleName)
			breadcrumbs appendSeq("</a>\n&nbsp;&nbsp;<font color=#ccc>/</font>&nbsp;&nbsp;\n<b>")
			breadcrumbs appendSeq(protoName)
			breadcrumbs appendSeq("</b>")
			description := descriptionHtml(p)
			slots := slotsHtml(protoName, p at("slots"))
			title := protoName
			cssPath := "../../../../docs.css"

			outFile write(protoTemplate asMutable interpolateInPlace)
			outFile close
		)
	)
)
