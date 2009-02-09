#!/usr/bin/env io



write("""
<html>
<head>
    <link rel="stylesheet" href="http://www.iolanguage.com/site.css">
</head>
<body>
<ul>
""")


template := """
<table cellpadding=3 cellspacing=0 border=0>
<tr>
<td bgcolor=@color@ width=@length@><font color=@fontColor@>@name@ @time@</font></td>
</tr>
</table>
"""

template := """
<table cellpadding=3 cellspacing=0 border=0>
<tr>
<td bgcolor=@color@ width=45><font color=@fontColor@>@name@</td>
<td bgcolor=@color@ width=30 align=right><font color=@fontColor@>@time@</td>
<td bgcolor=@color@ width=@length@>&nbsp;</td>
</tr>
</table>
"""
//<td><font color=#f2f2f2>@time@</td>


slotNames := list(
	"localAccesses", 
	"localSets",  
	"slotAccesses", 
	"slotSets", 
	"blockActivations", 
	"instantiations")
	
SpeedTestResult := Object clone do(
    name ::= ""
    color ::= "#888888"
    fontColor ::= "#aaaaaa"

    showBarFor := method(slotName, scale, 
	value := self getSlot(slotName)
	t := template asBuffer
	t replaceSeq("@name@", name)
	t replaceSeq("@time@", value asString(0, 1) )
	t replaceSeq("@length@",  ((value * 300 / scale) - 75) asString(0, 0))
	t replaceSeq("@color@",  color)
	t replaceSeq("@fontColor@",  fontColor)
	write(t, "\n\n")
    )
)

Io := SpeedTestResult clone setName("Io")
Io setColor("#9999cc")
Io setFontColor("#bbbbee")

Python := SpeedTestResult clone setName("Python")
Ruby   := SpeedTestResult clone setName("Ruby")
languages := list(Io, Python, Ruby)

doFile(Path with(System launchPath, "SpeedResults")) 

write("<br>Values are in millions of operations per second, so larger values are better.")

slotNames foreach(slotName,
    write("<p>", slotName, "<ul><table cellspacing=1 cellpadding=0 border=0>")
    languages foreach(language,
	write("<tr><td>")
	language showBarFor(slotName, languages at(1) getSlot(slotName))
	write("</td></tr>")
    )
    write("</table></ul>")
)

write("<p>versions<p><ul>")

languages foreach(language,
  write(language name, " ", language version, "<br>")
)

write("</ul>")



write("</ul>")


//System system("machine")


