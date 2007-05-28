#!/usr/bin/env io


SGMLTag do(
	links := method(
		links := List clone
		self addLinksTo(links)
		links
	)
	
	addLinksTo := method(links,
		if (name == "a", links append(self))
		subitems foreach(v, v ?addLinksTo(links))
	)
)

GooglePage := Object clone do(
    type := "GooglePage"
    //init := method(resend)
    newSlot("links", List clone)
    find := method(s, pageNumber,
	//s := s replace(" ", "+")

        // http://www.google.com/search?q=iolanguage&num=100&hl=en&lr=&safe=off&start=300&sa=N

	u := "http://www.google.com/search?q=" .. (s) .. "&num=100&hl=en&lr=&safe=off&start=" .. (pageNumber * 100)
	u = u  .. "&sa=N"
	write("page ", pageNumber, " ", u, " ")
	url := URL clone setURL(u)

	setLinks(url fetch asSGML links)

	links mapInPlace(i, link, link attributes at("href"))
	//links foreach(v, writeln(v))

	links selectInPlace(i, href, 
	    href beginsWithSeq("http") and (href containsSeq("cache") == nil and href containsSeq("google") == nil)
	) 

	writeln(links size, " links")

	self
    )
)


GoogleSearch := Object clone do(
    find := method(s,
		self links := List clone
		for(i, 0, 100,
			p := GooglePage clone find(s, i)
			links appendSeq(p links)
			if (p links size == 0, break)
		)
		
		writeln(links size, " google links found for '", s, "'")
		//links foreach(link, writeln("  ", link attributes at("href")))
    )
)

gs := GoogleSearch clone 
gs find("iolanguage") //args at(0))

while (Scheduler activeActorCount > 1, yield)



