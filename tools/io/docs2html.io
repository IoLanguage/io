Directory with("addons") folders foreach(folder,
	write(folder name)
	e := try(
		Lobby perform(folder name asSymbol)
	)
	if(e, writeln(""), writeln(" (error)"))
)

protos := Protos slotValues map(slotValues) flatten unique select(!=nil) sortByKey(type asLowercase)

protos foreach(p,
	if(p hasLocalSlot("docs") not, p docs := Object clone do(slots := Object clone))
	p foreachSlot(k, v, 
		if(p docs slots getSlot(k) == nil, 
			p docs slots setSlot(k, Object clone do(description := "<font color=red>undocumented</font>"))
			if(getSlot("v") type == Block, p docs slots getSlot("k") args := v argumentNames)
		) 
	)
)

writeln("""
<html>
<head>
<title>io - core reference</title>
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
    color: #000000;
    font-family: 'Courier', 'Courier';
    font-size: .85em;
    font-style: normal;
    font-variant: normal;
    font-weight: normal;
    letter-spacing: 0;
    line-height: 1.22;
    margin-bottom: 1em;
    margin-left: 2em;
    margin-right: 0.00em;
    margin-top: 1em;
    padding-bottom: 0.000000em;
    padding-top: 0.000000em;
    text-align: left;
    text-decoration: none;
    text-indent: 0.00em;
    text-transform: none;
    vertical-align: 0.000000em;
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
writeln("<h1>Io Core Reference Manual</h1>")
writeln("<div class=Version>Version " .. System version .. "</div>")
writeln("<h2>Prototypes</h2>")
writeln("<ul>")

protos map(v,
	writeln("<a href=#", v type, " style=\"color: #555;\">", v type, "</a><br>")	
)

writeln("</ul>")

Sequence do(
	asHtml := method(
		self asMutable replaceSeq(">", "&gt;") replaceSeq("<", "&lt;")
	)
)

protos map(v, 
	write("<h2>")
	write("<a name=" .. v type .. "><font color=black>", v type, "</font></a>")
	writeln("</h2>")
	writeln("<ul style=\"width:40em\">")
	
	writeln("<b><font color=#000>Protos:</font></b> ", v protos map(type) join(", "))
	
	if (v hasLocalSlot("docs"),
		if (v docs ?description,
			writeln("<h3>Description</h3>")
			//writeln("<font face=\"Times\">")
			v docs description println
			writeln("</font>")
		)
		
		if (v docs ?slots,
			writeln("<h3>Slot Index</h3>")
			writeln("<div style=\"width:40em; margin-left:2em\">")
			v docs slots foreachSlot(k, s, 
				write("<b><a href=#" .. v type .. "-" .. k asHtml .. " >")
				write(k asHtml)
				if(s ?args, write("()"))
				//if(s ?args, write("(" .. s args join(",") .. ")"))
				writeln("</a></b><br>")
			)
			writeln("</div>")

			writeln("<br>")
			writeln("<h3>Slots</h3>")
			//writeln("<ul>")
			writeln("<br>")
			v docs slots foreachSlot(k, s, 
				write("<b>")
				write("<a name=" .. v type .. "-" .. k asHtml .. "><font color=black>")
				write(k asHtml)
				if(s ?args, writeln("(</b><i>" .. s args map(asHtml) join(", ") .. "</i><b>)"))
				write("</font></a></b>")
				writeln("<p>")
				writeln("<div style=\"width:40em; margin-left:2em\">")
				//writeln("<font face=\"Times\">")
				writeln(s description)
				writeln("</font>")
				writeln("</div>")
				writeln("<p><br>")
			)
			//writeln("</ul>")
		)
	)
	
	writeln("</ul>")
	writeln("<br>")
	writeln("<hr align=left>")
	writeln("<br>")
)

writeln("</ul>")
5 repeat(writeln("<br>"))

