#!/usr/bin/env io

SGML

GooglePage := Object clone do(
    links ::= nil
    find := method(s, pageNumber,
		u := "http://www.google.com/search?q=" .. s .. "&num=100&hl=en&lr=&safe=off&start=" .. (pageNumber * 100) .. "&sa=N"
		//writeln("page ", pageNumber, " ", u, " ")
		setLinks(URL with(u) fetch asSGML linkStrings)
		links selectInPlace(beginsWithSeq("http"))
		links selectInPlace(containsSeq("cache") == false)
		links selectInPlace(containsSeq("google") == false)
		links selectInPlace(containsSeq("http://www.blogger.com/") == false)
		links selectInPlace(containsSeq("http://www.orkut.com/") == false)	
		self
    )
)

GoogleSearch := Object clone do(
    links ::= nil
    find := method(s,
		setLinks(List clone)
		for(i, 0, 100,
			p := GooglePage clone find(s, i)
			links appendSeq(p links)
			if (p links size == 0, break)
		)
		self
    )
)

GoogleSearch clone find(System args at(1)) links foreach(println)

while(Coroutine yieldingCoros size > 1, yield)



