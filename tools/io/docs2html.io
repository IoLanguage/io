#!/usr/local/bin/io

// This code is a mess...

docsPath := System args at(1)
addonFolders := Directory with("addons") directories
addonFolders foreach(folder,
	System system("io tools/io/DocsExtractor.io " .. folder path)
)

// Parse the libs/iovm directory to generate the Core documentation
// Any reason this was missing ? (Added 2010-09-019 rC)
System system("io tools/io/DocsExtractor.io " .. "libs/iovm")

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

addonFolders foreach(f, readFolder(f path))
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

// CATEGORIES -------------------------------------------------------

outFile := File with(Path with(docsPath, "index.html")) remove open
outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
<title>Io Reference</title>
<META HTTP-EQUIV="EXPIRES" CONTENT=0>
<link rel="stylesheet" href="../docs.css"/>
<script src="../js/jquery.js" type="text/javascript"></script>
<script src="../js/browser.js"></script>
</head>
<body>
""")
outFile writeln("<br>")
outFile writeln("<h1>Io Reference</h1>")
//outFile writeln("<div class=Version>version " .. System version asString asMutable atInsertSeq(4, " ") atInsertSeq(7, " "))
//outFile writeln(" generated on " .. Date clone now asString("%Y %m %d") .. "</div>")
outFile writeln("<br><br><br><br><br>")

outFile writeln("<table id=\"browser\" cellpadding=\"0\" cellspacing=\"0\">")
outFile writeln("<tr id=\"categories-column\">")
outFile writeln("<td id=\"categories\" class=\"column\" valign=\"top\">")
outFile write(Categories linksHtml)
outFile writeln("</td>")
outFile writeln("</tr>")
outFile writeln("</table>")
outFile close

// CATEGORY -------------------------------------------------------

Categories sortedItems foreach(cat, 
	cat sortedItems foreach(addon,
		outFile := Directory with(Path with(docsPath, cat name)) createIfAbsent fileNamed("index.html") remove open
		outFile writeln("""
		<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
		<html>
		<head>
		<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
		<title>Io Reference</title>
		<META HTTP-EQUIV="EXPIRES" CONTENT=0>
		<link rel="stylesheet" href="../../docs.css">
		</head>
		<body>
		""")
		outFile writeln("<br>")
		outFile writeln("<h1>Io Reference</h1>")
		outFile writeln("<br><br><br><br><br>")
		outFile writeln("<table id=\"browser\" cellpadding=\"0\" cellspacing=\"0\">")
		outFile writeln("<tr>")
		outFile writeln("<td valign=\"top\" class=\"column\">")
		outFile write(Categories setPath("..") linksHtml(cat name))
		outFile writeln("</td>")
		outFile writeln("<td valign=\"top\" class=\"column\">")
		outFile write(cat linksHtml)
		outFile writeln("</td>")	
		outFile writeln("</tr>")
		outFile writeln("</table>")
  	outFile writeln("</body>")
  	outFile writeln("</html>")
		outFile close
	)
)


// ADDON -------------------------------------------------------

Categories sortedItems foreach(cat,
	cat sortedItems foreach(addon,
		addon sortedItems foreach(proto,
			outFile := Directory with(Path with(docsPath, cat name, addon name)) createIfAbsent fileNamed("index.html") remove open
			outFile writeln("""
			<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
			<html>
			<head>
			<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
			<title>Io Reference</title>
			<META HTTP-EQUIV="EXPIRES" CONTENT=0>
			<link rel="stylesheet" href="../../../docs.css">
			</head>
			<body>
			""")
			outFile writeln("<br>")
			outFile writeln("<h1>Io Reference</h1>")
			outFile writeln("<br><br><br><br><br>")
			outFile writeln("<table id=\"browser\" cellpadding=\"0\" cellspacing=\"0\">")
			outFile writeln("<tr>")
			outFile writeln("<td valign=\"top\" class=\"column\">")
			outFile write(Categories setPath("../..") linksHtml(cat name))
			outFile writeln("</td>")
			outFile writeln("<td valign=\"top\" class=\"column\">")
			outFile writeln(cat setPath("..") linksHtml(addon name))
			outFile writeln("</td>")
			outFile writeln("<td valign=\"top\" class=\"column\">")
			outFile write(addon linksHtml)
			outFile writeln("</td>")	
			outFile writeln("</tr>")
			outFile writeln("</table>")
			outFile writeln("</body>")
			outFile writeln("</html>")
			outFile close
		)
	)
)

// PROTOS -------------------------------------------------------

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)

Categories sortedItems foreach(cat,
	cat sortedItems foreach(addon,
		addon items foreach(protoName, p,
	
	//writeln("protoName = ", protoName)
	//p := prototypes at(protoName)
	moduleName = p at("module")
	outFile := Directory with(Path with(docsPath, cat name, moduleName, protoName)) createIfAbsent fileNamed("index.html") remove open
	
	outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
		<html>
		<head>
		<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
		<title>""" .. protoName .. """</title>
		<META HTTP-EQUIV="EXPIRES" CONTENT=0>
		<link rel="stylesheet" href="../../../../docs.css">
		</head>
		<body>
		""")
		
		
	//----------------------------------------------
	outFile writeln("<br>")
	outFile writeln("<h1>Io Reference</h1>")
	//outFile writeln("<br><br><br><br><br>")
/*
	outFile writeln("<table id=\"browser\" cellpadding=\"0\" cellspacing=\"0\">")
	outFile writeln("<tr>")
	outFile writeln("<td valign=\"top\" class=\"column\">")
	//outFile write(Categories setPath("../..") linksHtml(cat name))
	outFile write("<b>", cat name, "</b>", "&nbsp;&nbsp;<b><font color=#ccc>&gt;</font>")
	
	outFile writeln("</td>")
	outFile writeln("<td valign=\"top\" class=\"column\">")
//	outFile writeln(cat setPath("..") linksHtml(addon name))
	outFile write("<b>", addon name, "</b>", "&nbsp;&nbsp;<b><font color=#ccc>&gt;</font>")
	outFile writeln("</td>")
	outFile writeln("<td valign=\"top\" class=\"column\">")
	//outFile write(addon linksHtml(protoName))
	outFile write("<b>", protoName, "</b>")
	outFile writeln("</td>")	
	outFile writeln("</tr>")
	outFile writeln("</table>")
	outFile writeln("</body>")
	outFile writeln("</html>")
*/
	//----------------------------------------------
		
	//outFile writeln("<hr align=left color=#ccc height=1>")
		
	//	outFile writeln("<hr align=left color=#ddd height=1>")
	outFile writeln("<br><br><br>")
	outFile writeln("<br><br><br>")
	outFile writeln("<a class='column' href='../../index.html'>", cat name, "</a>")
	outFile writeln("&nbsp;&nbsp;<font color=#ccc>/</font>&nbsp;&nbsp;")
	outFile writeln("<a class='column' href='../index.html'>", moduleName, "</a>")
	outFile writeln("&nbsp;&nbsp;<font color=#ccc>/</font>&nbsp;&nbsp;")
	outFile writeln("<b>", protoName, "</b>")
	//outFile writeln("<h1>", protoName asMutable strip, " Proto</h1>")
	//outFile writeln("<br><br><br><br><br><br>")
	outFile writeln("<br><br><br>")
	outFile writeln("<br><br><br>")

	outFile writeln("<table border=0 cellspacing=0 style=\"margin-left:8em; width:40em; line-height:1.2em;\">")
	
	showSection := method(name, value,
		outFile writeln("<tr>")
		//outFile writeln("<td align=right><b>", name, "</b></td>")
		outFile writeln("<td align=right></td>")
		outFile writeln("<td></td>")
		outFile writeln("<td>", value, "</td></tr>")
	)
	
	showSpacer := method(
		outFile writeln("<tr><td colspan=3>&nbsp;</td></tr>")
	)
	
	//outFile writeln("<b>Protos:</b> ", getSlot(protoName) ?prototypes ?map(type) ?join(", "))

	showMeta := method(name,
		if(p at(name), showSection("<span class=proto" .. name asCapitalized .. ">" .. name .. "</span>", p at(name)))
	)
	
	//showMeta("module")	
	//showMeta("category")
	//showMeta("copyright")
	//showMeta("license")
	//showMeta("credits")
	showMeta("description")
	//p keys map(s, "[" .. s .. "]") println

	showSpacer
	showSpacer

	slots := p at("slots")
	if (slots,
		slotNames := slots keys //sort
		/*
		outFile writeln("<tr>")
		outFile writeln("<td align=right>")
		//outFile writeln("<h3>index</h3>")
		outFile writeln("</td>")
		outFile writeln("<td></td>")
		outFile writeln("<td>")
		
		//outFile writeln("<div class=slotIndex>")
		
		
		try(
			if(Lobby perform(protoName),
				names := getSlot(protoName) slotNames remove("type") select(beginsWithSeq("_") not)
				slotNames foreach(slotName,
					n := slotName beforeSeq("(") asSymbol 
					if(names contains(n), names remove(n))
				)
				slotNames appendSeq(names)
			)
		)
		
		slotNames = slotNames sort remove("init")
		
		slotNames foreach(k,
			s := slots at(k)
			outFile writeln("<a href=\"#" .. protoName .. "-" .. k beforeSeq("(") asHtml .. "\">")
			if(k containsSeq("("), k = k beforeSeq("(") .. "()")
			outFile writeln(k asHtml)
			if(s ?args, outFile writeln("()"))
			outFile writeln("</a><br>")
		)

		outFile writeln("</td>")
		outFile writeln("</tr>")
*/
		showSpacer
		
		outFile writeln("<tr>")
		outFile writeln("<td align=right>")
		//outFile writeln("<h3>slots</h3>")
		outFile writeln("</td>")
		outFile writeln("<td></td>")
		outFile writeln("<td>")
		if(slots size > 0, outFile writeln("<hr align=left color=#ddd height=1>"))
		outFile writeln("<br><br>")
		//slotNames = slotNames map(asMutable strip asSymbol)
		
		slotNames sort foreach(k,
			s := slots at(k)
			if(s, s = s strip)
			isPrivate := s ?beginsWithSeq("Private")
			if(isPrivate, outFile writeln("<font color=#888>"))
			isDeprecated := s ?beginsWithSeq("Deprecated")
			if(isDeprecated, outFile writeln("<font color=#55a>"))
			outFile writeln("<a name=\"" .. protoName .. "-" .. k beforeSeq("(") asHtml .. "\"></a><b>")
			outFile writeln(k asHtml)
			//if(s ?args, outFile writeln("(</b><i>" .. s args map(asHtml) join(", ") .. "</i><b>)"))
			outFile writeln("</b>")
			outFile writeln("<p>")
			outFile writeln("<div class=slotDescription>")
			if(s, outFile writeln(s), outFile writeln("<div class=error>undocumented</div>"))
			if(isPrivate or isDeprecated, outFile writeln("</font>"))
			outFile writeln("</div>")
		)
	)
	outFile writeln("</td>")
	outFile writeln("</tr>")	

	outFile writeln("</table>")
	outFile writeln("<br><br><br><br><br>")
	outFile writeln("</body>")
	outFile writeln("</html>")
	outFile close
	//System exit
)))
