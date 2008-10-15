#!/usr/bin/env io


template := """
<table cellpadding=3 cellspacing=0 border=0>
<tr>

<td bgcolor=#888888 width=70>
<font color=white face="helvetica, arial, sans">
@name@ 
</td>

<td bgcolor=@color@ width=@length@>
<font color=white face="helvetica, arial, sans">
<font color=@fontColor@>@time@</font>
</td>

</tr>
</table>
"""


Io := Object clone
Python := Object clone

s := File setPath(Path with(System launchPath, "SpeedResults")) asString

doString(s)

slotNames := list("localAccesses", "localSets",  "slotAccesses", "slotSets", "blockActivations", "instantiations")

show := method(name, value, color, fontColor, 
	t := template asBuffer
	t replace("@name@", name)
	t replace("@time@", value asString(0, 1) )
	t replace("@length@",  (value * 30) asString(0, 0))
	t replace("@color@",  color)
	t replace("@fontColor@",  fontColor)
	write(t, "\n\n")
)

slotNames foreach(slot,
	write("<table cellpadding=0 cellspacing=1 border=0><tr>")
	write("<td rowspan=2 bgcolor=#888888 width=120><font color=white face=\"helvetica, arial, sans\">&nbsp;&nbsp;", slot, "</td><td>")
	show("Io",     Io getSlot(slot),     "#9999cc", "#bbbbee")
	show("Python", Python getSlot(slot), "#888888", "#aaaaaa")
	write("</td></tr></table>\n")
)


