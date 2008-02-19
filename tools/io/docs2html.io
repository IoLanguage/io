#!/usr/local/bin/io

addonFolders := Directory with("addons") folders
addonFolders foreach(folder,
	System system("io tools/io/DocsExtractor.io " .. folder path)
)

prototypes := Map clone
modules := Map clone


/*
ReferenceDoc := Object clone do(
	protos :: = List clone
	print := method(
		protos foreach(printIndex)
		protos foreach(print)
	)
)

ProtoDoc := Object clone do(
	init := method(self slots := List clone)
	printIndex := method(
		
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
		
	)
	print := method(
		
	)
)
*/

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
		
			if(protoName == nil or slotName == nil, writeln("ERROR: ", e))
			if(isSlot, 
				p at("slots") atPut(slotName, description)
			,
				p atPut(slotName, description)
			)
		,
			if(protoName == nil or slotName == nil, writeln("ERROR: ", e))
		)
		
	)
)

addonFolders foreach(f, readFolder(f path))
readFolder("libs/iovm")

writeln("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8">
<title>Io Reference Manual</title>
<META HTTP-EQUIV="EXPIRES" CONTENT=0>
<link rel="stylesheet" href="docs.css">
</head>
<body>
""")

writeln("<h1>The Io Reference Manual</h1>")
writeln("<div class=Version>Version " .. System version asString asMutable atInsertSeq(4, " ") atInsertSeq(7, " ") .. "</div>")
writeln("<p><br>")
//writeln("<h2>Modules</h2>")

protoNames := prototypes keys sort
moduleNames := modules keys sort 

moduleNames remove("Core") prepend("Core")

// 	<div class=indexSection><a href=""#Objects"">Objects</a></div>

writeln("<table cellpadding=0 cellspacing=0 border=0>")
writeln("<tr><td valign=top>")
count := 0
moduleNames foreach(moduleName,
	writeln("<div class=indexSection><a href=\"#", moduleName, "\">", moduleName, "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>")
	keys := modules at(moduleName) keys sort 
	if(keys size > 1,
		keys foreach(protoName,
			writeln("<div class=indexItem><a href=\"#", protoName, "\">", protoName, "</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>")
			count = count + 1
		)
	)
	count = count + 2
	if(count > 53,
		writeln("</td><td valign=top>")
		count = 0
	)
	//writeln("""<br style="width:.5em"></div>""")
)

writeln("</td></tr></table>")
writeln("<p><br>")

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)

protoNames foreach(protoName,
	p := prototypes at(protoName)
	writeln("<hr align=left>")
	writeln("<h2><a name=\"" .. protoName .. "\"></a>", protoName, "</h2>")
	writeln("<div class=indent>")
	
	//writeln("<b>Protos:</b> ", getSlot(protoName) ?prototypes ?map(type) ?join(", "))

	showMeta := method(name,
		if(p at(name),
			writeln("<b>", name asCapitalized, ":</b> ", p at(name), "<br>")
		)
	)
	
	writeln("""<div style="line-height:1.3em">""")
	showMeta("module")
	showMeta("category")
	writeln("<font color=#bbb>")
	//showMeta("copyright")
	//showMeta("license")
	showMeta("credits")
	writeln("</font>")
	writeln("</div>")
	
	//p keys map(s, "[" .. s .. "]") println

	if (p at("description"),
		writeln("<h3>Description</h3>")
		writeln("<div class=protoDescription>")
		p at("description") println
		writeln("</div>")
	)

	slots := p at("slots")
	if (slots,
		writeln("<div class=slots>")
		writeln("<h3>Slot Index</h3>")
		writeln("<div class=slotIndex>")
		
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
			write("<a href=\"#" .. protoName .. "-" .. k beforeSeq("(") asHtml .. "\">")
			if(k containsSeq("("), k = k beforeSeq("(") .. "()")
			write(k asHtml)
			if(s ?args, write("()"))
			writeln("</a><br>")
		)
		writeln("</div>")

		writeln("<p>")
		writeln("<h3>Slots</h3>")
		writeln("<p>")
		//slotNames = slotNames map(asMutable strip asSymbol)
		
		slotNames sort foreach(k,
			s := slots at(k)
			if(s, s = s strip)
			isPrivate := s ?beginsWithSeq("Private")
			if(isPrivate, writeln("<font color=#888>"))
			isDeprecated := s ?beginsWithSeq("Deprecated")
			if(isDeprecated, writeln("<font color=#55a>"))
			write("<a name=\"" .. protoName .. "-" .. k beforeSeq("(") asHtml .. "\"></a><b>")
			write(k asHtml)
			//if(s ?args, writeln("(</b><i>" .. s args map(asHtml) join(", ") .. "</i><b>)"))
			write("</b>")
			writeln("<p>")
			writeln("<div class=slotDescription>")
			if(s, writeln(s), writeln("<div class=error>undocumented</div>"))
			if(isPrivate or isDeprecated, writeln("</font>"))
			writeln("</div>")
		)
		writeln("</div>")
	)
	
	writeln("</div>")
)

5 repeat(writeln("<br>"))

