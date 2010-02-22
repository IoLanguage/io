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
<title>Io Reference Manual</title>
<META HTTP-EQUIV="EXPIRES" CONTENT=0>
<link rel="stylesheet" href="../docs.css">
</head>
<body>
""")

outFile writeln("<h1>Io Reference Manual</h1>")
outFile writeln("<div class=Version>version " .. System version asString asMutable atInsertSeq(4, " ") atInsertSeq(7, " "))
outFile writeln(" generated on " .. Date clone now asString("%Y %m %d") .. "</div>")
outFile writeln("<p><br>")

protoNames := prototypes keys sort
moduleNames := modules keys sort 

moduleNames remove("Core") prepend("Core")


outFile writeln("<table cellpadding=0 cellspacing=0 border=0>")
outFile writeln("<tr><td valign=top>")
count := 0
moduleNames foreach(moduleName,
	outFile writeln("<div class=indexSection><a href=\"", moduleName .. ".html", "\">", moduleName, "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>")
	keys := modules at(moduleName) keys sort 
	if(keys size > 1,
		keys foreach(protoName,
			//outFile writeln("<div class=indexItem><a href=\"#", protoName, "\">", protoName, "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>")
			outFile writeln("<div class=indexItem><a href=\"", protoName .. ".html", "\">", protoName, "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>")
			count = count + 1
		)
	)
	count = count + 2
	if(count > 53,
		outFile writeln("</td><td valign=top>")
		count = 0
	)
	//outFile writeln("""<br style="width:.5em"></div>""")
)

outFile writeln("</td></tr></table>")
outFile writeln("<p><br>")

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)


protoNames foreach(protoName,
	outFile close
	outFile := File with(Path with(docsPath, protoName .. ".html")) open
	
		outFile writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
		<html>
		<head>
		<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
		<title>Io Reference Manual</title>
		<META HTTP-EQUIV="EXPIRES" CONTENT=0>
		<link rel="stylesheet" href="../docs.css">
		</head>
		<body>
		""")
		
	p := prototypes at(protoName)
//	outFile writeln("<hr align=left color=#ddd height=1>")
	outFile writeln("<br><br><br>")
	outFile writeln("<table cellspacing=4 style=\"width:45em; line-height:1.2em;\">")
	
	showSection := method(name, value,
		outFile writeln("<tr>")
		outFile writeln("<td align=right><b>", name, "</b></td>")
		outFile writeln("<td></td>")
		outFile writeln("<td>", value, "</td></tr>")
	)
	
	showSpacer := method(
		outFile writeln("<tr><td colspan=2>&nbsp;</td></tr>")
	)
	
	outFile writeln("<tr>")
	outFile writeln("<td>")
	outFile writeln("<a name=" .. protoName .. "></a><span class=protoName>", protoName, "</span>")
	outFile writeln("</td>")
	outFile writeln("<td></td>")
	outFile writeln("<td>")
	outFile writeln("</tr>")
	
	//outFile writeln("<b>Protos:</b> ", getSlot(protoName) ?prototypes ?map(type) ?join(", "))

	showMeta := method(name,
		if(p at(name), showSection("<span class=proto" .. name asCapitalized .. ">" .. name .. "</span>", p at(name)))
	)
	
	showMeta("module")	
	showMeta("category")
	//showMeta("copyright")
	//showMeta("license")
	showMeta("credits")
	showMeta("description")
	//p keys map(s, "[" .. s .. "]") println

	showSpacer

	slots := p at("slots")
	if (slots,
		outFile writeln("<tr>")
		outFile writeln("<td align=right>")
		outFile writeln("<h3>index</h3>")
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

		showSpacer
		
		outFile writeln("<tr>")
		outFile writeln("<td align=right>")
		outFile writeln("<h3>slots</h3>")
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
	
	//outFile writeln("</td>")
	//outFile writeln("</tr>")	
	//outFile writeln("</table>")
	//break
)

5 repeat(outFile writeln("<br>"))

outFile close
