#!/usr/local/bin/io

System system("pdftohtml -c -noframes -nomerge -i IoGuide.pdf") 

file := File with("IoGuide.html") 
c := file contents

c replaceSeq(".ft9{font-size:11px;font-family:Times;color:#000000;}",
".ft9{font-size:12px;font-family:Courier;color:#000000;}")

c replaceSeq("font-family:Times", "font-family:Helvetica")
c replaceSeq("""<BODY bgcolor="#A0A0A0" vlink="blue" link="blue">""",
"""<BODY bgcolor="#fff" vlink="blue" link="blue">""")

file setContents(c)
