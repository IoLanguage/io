//Date: Mon, 23 May 2005 22:38:34 GMT
//Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT

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
pdb sync
pdb close

Server clone setHost("127.0.0.1") setPort(8000) do(
	handleSocket := method(aSocket,
		aSocket streamReadNextChunk
		url := aSocket readBuffer afterSeq("GET ") beforeSeq(" HTTP/")
		parts := url splitNoEmpties("/")
		pdb open
		obj := PDB root
		parts foreach(p, obj = obj at(p))
		if(obj,
			//if(obj type == "PMap",
				keys := obj firstCount(1000)
				s := Sequence clone
				//writeln("keys = ", keys)
				s appendSeq("""
					<html>
					<head>
					    <title>steve dekorte - blog</title>
					    <link rel="stylesheet" href="http://dekorte.com/site.css">
					    <META HTTP-EQUIV="EXPIRES" CONTENT=0>
					</head>
					<body>
				""")
				s appendSeq("<ul>")
				s appendSeq("<table cellspacing=3>")
				keys foreach(k,
					s appendSeq("<tr><td align=right><b>")
					s appendSeq("<a href=" .. url afterSeq("/") .. "/" .. k .. ">" .. k .. "</a>")
					//s appendSeq("</td><td>")
					//s appendSeq(obj at(k))
					//s appendSeq("</td><td>")
					//s appendSeq("<a href=" .. url afterSeq("/") .. "/" .. k .. ">&gt;</a>")
					s appendSeq("</td></tr>")
				)
				s appendSeq("</table>")
				s appendSeq("</ul>")
			, 
			s := "nil"
		)
		aSocket write(Response setContent(s) data)
		aSocket close
		pdb close
	)
) start
