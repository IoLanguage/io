
addonFolders := Directory with("addons") folders
addonFolders foreach(folder,
	System system("io tools/io/DocsExtractor.io " .. folder path)
)

prototypes := Map clone
modules := Map clone

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

writeln("""
<html>
<head>
<title>Io Reference Manual</title>
<META HTTP-EQUIV="EXPIRES" CONTENT=0>
<style>
a 
{
	color : #aaa;
	text-decoration : none;
}

body {
    font-family: 'Serif', 'Helvetica Neue', 'Helvetica', 'Sans';
}

ul {
	padding: 0em 0em 0em 3.1em;
}

hr {
	width:50em;
	height:0em;
}

.Version {
    color: #bbb;
    text-align: left;
}

h1 {
    color: #000000;
    font-family: 'Helvetica-Bold', 'Helvetica';
    font-size: 3em;
    font-style: normal;
    font-variant: normal;
    font-weight: bold;
    letter-spacing: 0;
    line-height: 1.3;
    margin-bottom: 0em;
    margin-left: 0em;
    margin-right: 0em;
    margin-top: 0em;
    padding-bottom: 0em;
    padding-top: 2em;
    text-align: left;
    text-decoration: none;
    text-indent: 0.00em;
    text-transform: none;
    vertical-align: 0.000000em;
}

pre {
	white-space: pre;
    color: #333;
    font-family: 'Courier', 'Courier';
    font-size: .9em;
    font-style: normal;
    font-variant: normal;
    font-weight: normal;
    letter-spacing: 0;
    line-height: 1.22;

    margin-bottom: 1.5em;
    margin-left: 2em;
    margin-right: 0em;
    margin-top: 1.5em;
    padding-bottom: 0em;
    padding-top: 0em;

    text-align: left;
    text-decoration: none;
    text-indent: 0em;
    text-transform: none;
    vertical-align: 0em;
}

h2 {
    color: #000000;
    font-family: 'Helvetica', 'Helvetica';
    font-size: 1.8em;
    font-style: normal;
    font-variant: normal;
    font-weight: bold;
    letter-spacing: 0;
    line-height: 1.21;
    margin-bottom: .5em;
    margin-left: 0em;
    margin-right: 0.00em;
    margin-top: 0.000000em;
    padding-bottom: 7.000000pt;
    padding-top: 21.000000pt;
    text-align: left;
    text-decoration: none;
    text-indent: 0.00em;
    text-transform: none;
    vertical-align: 0.000000em;
}

h3 {
    color: #777;
    font-family: 'Helvetica-Bold', 'Helvetica';
    font-size: 1.3em;
    font-style: normal;
    font-variant: normal;
    font-weight: bold;
    letter-spacing: 0;
    line-height: 1.18em;
    margin-bottom: 0em;
    margin-left: 0em;
    margin-right: 0em;
    margin-top: 0em;
    padding-bottom: 1em;
    padding-top: 1em;
    text-align: left;
    text-decoration: none;
    text-indent: 0.00em;
    text-transform: none;
    vertical-align: 0.000000em;
}

.PsuedoCode {
    color: #000000;
    font-family: 'Times-Italic', 'Times';
    font-size: 11.00pt;
    font-style: italic;
    font-variant: normal;
    font-weight: normal;
    letter-spacing: 0;
    line-height: 1.18;
    margin-bottom: 0.000000em;
    margin-left: 0em;
    margin-right: 0.00em;
    margin-top: 0.000000em;
    padding-bottom: 0.000000em;
    padding-top: 0.000000em;
    text-align: left;
    text-decoration: none;
    text-indent: 0.00em;
    text-transform: none;
    vertical-align: 0.000000em;
}

</style>
</head>
<body>
""")

writeln("<ul>")
writeln("<h1>Io Reference Manual</h1>")
writeln("<div class=Version>Version " .. System version .. "</div>")
writeln("<h2>Modules</h2>")
writeln("<ul>")

protoNames := prototypes keys sort
moduleNames := modules keys sort 

moduleNames remove("Core") prepend("Core")

moduleNames foreach(moduleName,
	writeln("<b>",moduleName, "</b><br>")
	writeln("""<div style="margin-left:1.5em">""")
	modules at(moduleName) keys sort foreach(protoName,
		writeln("<a href=#", protoName, " style=\"color: #555;\">", protoName, "</a><br>")
	)
	writeln("""<br style="width:.5em"></div>""")
)

writeln("</ul><br><br>")

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)

protoNames foreach(protoName,
	p := prototypes at(protoName)
	writeln("<hr align=left>")
	write("<br>")
	write("<h2>")
	write("<a name=" .. protoName .. "><font color=black>", protoName, "</font></a>")
	writeln("</h2>")
	writeln("<ul style=\"width:40em\">")
	
	//writeln("<b><font color=#000>Protos:</font></b> ", getSlot(protoName) ?prototypes ?map(type) ?join(", "))

	if(p at("module"), 
		writeln("<b><font color=#000>Module:</font></b> ", p at("module"), "<br>")
	)
	
	if(p at("category"),
		writeln("<b><font color=#000>Category:</font></b> ", p at("category"), "<br>")
	)
	

	//p keys map(s, "[" .. s .. "]") println

	if (p at("description"),
		writeln("<h3>Description</h3>")
		p at("description") println
		writeln("</font>")
	)

	slots := p at("slots")
	if (slots,
		writeln("<h3>Slot Index</h3>")
		writeln("<div style=\"width:40em; margin-left:2em\">")
		
		slotNames := slots keys sort
		
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
		
		slotNames = slotNames sort
		
		slotNames foreach(k,
			s := slots at(k)
			write("<b><a href=#" .. protoName .. "-" .. k asHtml .. " >")
			if(k containsSeq("("), k = k beforeSeq("(") .. "()")
			write(k asHtml)
			if(s ?args, write("()"))
			writeln("</a></b><br>")
		)
		writeln("</div>")

		writeln("<br>")
		writeln("<h3>Slots</h3>")
		writeln("<br>")
		//slotNames = slotNames map(asMutable strip asSymbol)
		
		slotNames sort foreach(k,
			s := slots at(k)
			write("<b>")
			write("<a name=" .. protoName .. "-" .. k asHtml .. "><font color=black>")
			write(k asHtml)
			//if(s ?args, writeln("(</b><i>" .. s args map(asHtml) join(", ") .. "</i><b>)"))
			write("</font></a></b>")
			writeln("<p>")
			writeln("<div style=\"width:40em; margin-left:2em\">")
			if(s, writeln(s), writeln("<font color=red>undocumented</font>"))
			writeln("</font>")
			writeln("</div>")
			writeln("<p><br>")
		)
		//writeln("</ul>")
	)
	
	writeln("</ul>")
	writeln("<br>")
)

writeln("</ul>")
5 repeat(writeln("<br>"))

