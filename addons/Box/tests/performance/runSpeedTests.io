
writeln("running Io tests...")
System system("./ioServer Math/_tests/speed.io > Math/_tests/SpeedResults.txt")

writeln("running Python tests...")
System system("python Math/_tests/speed.py >> Math/_tests/SpeedResults.txt")

Namespace := Object clone(
  forward := method(
    if (thisMessage arguments size == 0, self setSlot(thisMessage name, self clone))
  )
)

Languages := Object clone
Languages doFile("Math/_tests/SpeedResults.txt")

Object foreach := method(
	self slotNames sort foreach(slotName,
		sender setSlot(thisMessage argAt(0) name, slotName)
		sender setSlot(thisMessage argAt(1) name, self getSlot(slotName))
		sender doMessage(thisMessage argAt(2))
	)
)

writeln("""
<html><head>    <title>io - about - speed - ppc</title>    <link rel="stylesheet" href="http://www.iolanguage.com/site.css">    <META HTTP-EQUIV="EXPIRES" CONTENT=0></head><body>
""")


maxResultFor := method(testName,
	max := 0
	Languages foreach(langName, lang,
		m := lang getSlot(testName) asNumber
		if (m > max, max = m)
	)
	max
)

writeln("<br><br><ul>")
numberOfLanguages :=  Languages slotNames size
Languages Io foreach(testName, testResult,
	if (testName == "version", continue)
	writeln(testName, "<p>")
	//writeln("<ul>")
	tname := testName replace("minus", "-") replace("plus", "+") replace("times", "*")
	tname := tname replace("Equals", "=") 

	writeln("<table cellpadding=0 cellspacing=1 border=0>")
	//writeln("<tr>")
	//writeln("<table cellpadding=0 cellspacing=1 border=0>")

	maxResult := maxResultFor(testName)

	Languages foreach(langName, lang,
		if (langName == "Io") then(
			color    := "#ccccff"
			bgcolor  := "#9999cc"
			bgcolor2 := "#8888bb"

			color    := "#aaaacc"
			bgcolor  := "#9999cc"
			bgcolor2 := "#8888bb"

			color    := "#999999"
			bgcolor  := "#9999cc"
			bgcolor2 := "white"
		) else (
			color    := "#bbbbbb"
			bgcolor  := "#888888"			
			bgcolor2 := "#999999"		

			color    := "#999999"
			bgcolor  := "#888888"			
			bgcolor2 := "#777777"	
	
			bgcolor2 := "white"
		)


		writeln("<tr>")
		writeln("<td>")

		r := lang getSlot(testName) asNumber
		//w := ((r/maxResult) * 400)
		w := r * 120

		writeln("<table cellpadding=3 cellspacing=0 border=0>")
		writeln("<tr><td width=50 align=right bgcolor=", bgcolor2, "><font color=", color , ">", langName asLowercase, "&nbsp;</td>")
		if (w > 1,
		  writeln("<td bgcolor=", bgcolor, " width=", w, "></td>")
		)
		writeln("<td><font color=#eeeeee>", r, "</td>")
		writeln("</tr>")
		writeln("</table>")
		writeln("</td></tr>")
	)
	writeln("</td></tr></table>")
	//writeln("</ul><br>")
	writeln("<br><br>")
	//writeln("<br>")
)


writeln("versions<p>")
writeln("<ul>")
Languages foreach(langName, lang,
	writeln(langName, " ", lang version, "<br>")
)
writeln("</ul>")

writeln("</ul>")
