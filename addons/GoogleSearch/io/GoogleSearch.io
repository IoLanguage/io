#!/usr/bin/env io

SGML

GooglePage := Object clone do(
	//metadoc GooglePage category Networking
    links ::= nil
	pageNumber ::= 0
	searchTerm ::= ""
    find := method(
		u := "http://www.google.com/search?q=" .. URL escapeString(searchTerm) .. "&num=100&hl=en&lr=&safe=off&start=" .. (pageNumber * 100) .. "&sa=N"
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
	//metadoc GoogleSearch category Networking
    links ::= nil
	maxPages ::= 3
	searchTerm ::= ""
    find := method(
		setLinks(List clone)
		for(i, 0, maxPages - 1,
			p := GooglePage clone setSearchTerm(searchTerm) setPageNumber(i) find
			links appendSeq(p links)
			if (p links size == 0, break)
		)
		self
    )
)

//GoogleSearch clone setSearchTerm(System args at(1)) find links foreach(println)

//while(Coroutine yieldingCoros size > 1, yield)



