#!/usr/local/bin/io

docsPath := System args at(1)
addonFolders := Directory with("addons") directories
addonFolders foreach(folder,
	System system("io tools/io/DocsExtractor.io " .. folder path)
)

prototypes := Map clone
modules := Map clone


outFile := File with(docsPath .. "/index.html") open
File _write := File getSlot("write")
File writeln := method(call evalArgs foreach(s, self _write(s)); self _write("\n"); self)

readFolder := method(path,
	File with(Path with(path, "/docs/docs.txt")) contents split("------\n") foreach(e,
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

outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
<title>Io Reference</title>
<META HTTP-EQUIV="EXPIRES" CONTENT=0>
<link rel="stylesheet" href="../docs.css">
</head>
<body>
""")

outFile writeln("<br>")
	outFile writeln("<h1>Io Reference</h1>")
//outFile writeln("<div class=Version>version " .. System version asString asMutable atInsertSeq(4, " ") atInsertSeq(7, " "))
//outFile writeln(" generated on " .. Date clone now asString("%Y %m %d") .. "</div>")
outFile writeln("<br><br><br><br><br>")

protoNames := prototypes keys sort
moduleNames := modules keys sort 

moduleNames remove("Core") prepend("Core")

categories := Map clone
modules foreach(moduleName, module,
	firstProto := nil
	firstProtoName := module keys sort detect(k, module at(k) at("category"))
	if(firstProtoName, firstProto := module at(firstProtoName))
	if(firstProto,
		catName := firstProto at("category")
	, 
		writeln("warning: no cat for ", moduleName, " ", module keys)
		catName := "Other"
	)
	cat := categories atIfAbsentPut(catName asMutable strip, Map clone)
	cat atPut(moduleName, module)
)

// REF PAGE -------------------------------------------------------

outFile writeln("<table cellpadding=0 cellspacing=0 border=0 style=\"margin-left:13em; width:36em; border-style:solid; border-width:0px; border-color:#000;\">")
outFile writeln("<tr>")
outFile writeln("<td valign=top align=left width=33%>")
totalCount := 0
categories keys sort foreach(catName, 
	cat := categories at(catName)

	outFile write("<div class=indexSection><a href=>", catName, "</a></div>")
	totalCount = totalCount + 2
	
	cat keys sort foreach(moduleName,
		module := cat at(moduleName)
		outFile writeln("<div class=indexItem><a href=", moduleName, "/index.html>", moduleName, "</a></div>")
		//showModule(module)
		totalCount = totalCount + 1
	)
	if(totalCount > 29,
		outFile writeln("</td>")
		outFile writeln("<td valign=top align=left width=33%>")
		//outFile writeln("<td valign=top align=left style=\"width:10em;\">")
		totalCount = 0
	)
)
outFile writeln("</td>")
outFile writeln("</tr>")
outFile writeln("</table>")
outFile close


// MODULE INDEXES -------------------------------------------------------


count := 0
moduleNames foreach(moduleName,
	module := modules at(moduleName)
	
	Directory with(Path with(docsPath, moduleName)) createIfAbsent
	outFile := File with(Path with(docsPath, moduleName, "index.html")) open
	
	outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
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
	outFile writeln("<h1>", moduleName, " Addon</h1>")
	outFile writeln("<br><br><br>")

	outFile writeln("<div style=\"margin-left:10em;\">")
	outFile write("<div class=indexSection><a href=>Protos</a></div>")
	moduleProtoNames := module keys sort 
	count := 0
	//if(moduleProtoNames size > 1,
		moduleProtoNames foreach(protoName,
			outFile writeln("<div class=indexItem><a href=", protoName .. ".html", ">", protoName, "</a></div>")
			count = count + 1			
			if(count > 7,
				count = 0
			)
		)
	//)
	outFile writeln("</div>")
	outFile close
)


// PROTOS -------------------------------------------------------

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)


protoNames foreach(protoName,
	//writeln("protoName = ", protoName)
	outFile close
	p := prototypes at(protoName)
	moduleName = p at("module")
	Directory with(Path with(docsPath, moduleName)) createIfAbsent
	outFile := File with(Path with(docsPath, moduleName, protoName .. ".html")) open
	
	outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
		<html>
		<head>
		<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
		<title>""" .. protoName .. """</title>
		<META HTTP-EQUIV="EXPIRES" CONTENT=0>
		<link rel="stylesheet" href="../../docs.css">
		</head>
		<body>
		""")
		
//	outFile writeln("<hr align=left color=#ddd height=1>")
	outFile writeln("<br>")
	outFile writeln("<h1>", protoName asMutable strip, " Proto</h1>")
	outFile writeln("<br><br><br><br><br><br>")

	outFile writeln("<table border=0 cellspacing=0 style=\"margin-left:8em; width:45em; line-height:1.2em;\">")
	
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
		/*
		outFile writeln("<tr>")
		outFile writeln("<td align=right>")
		//outFile writeln("<h3>index</h3>")
		outFile writeln("</td>")
		outFile writeln("<td></td>")
		outFile writeln("<td>")
		
		//outFile writeln("<div class=slotIndex>")
		
		slotNames := slots keys //sort
		
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
	outFile close
)
