
// open with http://localhost:8000/

Response := Object clone do(
	header ::= """HTTP/1.1 200 OK
Server: Io
Accept-Ranges: bytes
Content-Length: #{content sizeInBytes}
Connection: close
Content-type: text/html
Cache-Control: no-store, no-cache, must-revalidate

#{content}"""
	
	content ::= ""
	
	data := method(
		s := header interpolate
		//writeln(s)
		s
	)
)

pdb := PDB setPath("web.tc") clone 
pdb delete
pdb open
pdb root atPut("a", "b")
	pdb root atPut("b", 1)
	p := PMap clone
	pdb root atPut("c", p)
	p atPut("foo", "bar")
pdb sync
pdb close

Server clone setHost("127.0.0.1") setPort(8000) do(
	handleSocket := method(aSocket,
		aSocket streamReadNextChunk
		self url := aSocket readBuffer afterSeq("GET ") beforeSeq(" HTTP/")
		self parts := url splitNoEmpties("/")
		if(parts contains("favicon.ico"), aSocket close; return)
		pdb open

		s := Sequence clone

		s appendSeq("""
			<html>
			<head>
			    <title>#{pdb path}</title>
			    <link rel="stylesheet" href="http://dekorte.com/site.css">
			    <META HTTP-EQUIV="EXPIRES" CONTENT=0>
			</head>
			<body>
		""" interpolate)
		
		objId := "root"
						
		dataSlot := nil
		
		
		parts slice(1) foreach(k,
			if(k beginsWithSeq("_"),
				dataSlot := k
				break
			)
			
			objId := pdb onAt(objId, k)
			//writeln("objId = ", objId)
			if(objId == nil, 
				writeln("no slot ", k, " on ", objId)
				return
			)
		)			

		if(dataSlot,
			s appendSeq(slotRep(objId, dataSlot))
		,
			s appendSeq(repForId(objId))
		)
		
		
		aSocket write(Response setContent(s) data)
		aSocket close
		pdb close
	)
	
	List before := method(v,
		i := self indexOf(v)
		if(i, self slice(0, i + 1), self clone)
	)
	
	pathString := method(
		s := Sequence clone
		parts foreach(p,
			u := parts before(p) join("/")
			s appendSeq("<a href=/" .. u .. "><span class=selectedMenu>" .. p .. "</span></a>")
			if(p != parts last, s appendSeq("<span class=menuDividerDark>&nbsp;&nbsp;&#8594;&nbsp;&nbsp;</span>"))
		)
		s
	)
	
	slotRep := method(objId, k,
		s := Sequence clone
		s appendSeq(pathString)
		s appendSeq("<ul>")
		//s appendSeq(objId .. " " .. k .. ":")
		s appendSeq("<p>")
		v := pdb onAt(objId, k)
		s appendSeq("<table cellspacing=1 cellpadding=1><tr><td bgcolor=#ccc>")
		s appendSeq("<table cellspacing=0 cellpadding=5><tr><td bgcolor=#eee>")
		s appendSeq(v)
		s appendSeq("</td></tr></table>")
		s appendSeq("</td></tr></table>")
		s appendSeq("</pre>")
		s appendSeq("</ul>")
		s		
	)

	repForId := method(objId,
		s := Sequence clone
		c := PDB sharedPrefixCursor
		c setPrefix(objId)
		s appendSeq(pathString)
		s appendSeq("<ul>\n")
		s appendSeq("<table cellspacing=3>\n")
	
		c first
		while(c key,
			k := c key
			s appendSeq("<tr><td align=left><b>")
			s appendSeq("<a href=/" .. url afterSeq("/") .. "/" .. k .. ">" .. k .. "</a>")
			s appendSeq("</b></td></tr>\n")
			c next
		)
	
		s appendSeq("</table>\n")
		s appendSeq("</ul>\n")
		s
	)
) start
