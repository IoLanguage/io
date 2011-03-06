#!/usr/bin/env io

SGML

GooglePage := Object clone do(
	//metadoc GooglePage category Networking
	//metadoc GooglePage description Object representing one page of search results.
	
    links ::= nil
	//doc GooglePage links Returns the list of link results.
	
	pageNumber ::= 0
	//doc GooglePage pageNumber Returns the current page number of results.
	
	searchTerm ::= ""
	//doc GooglePage searchTerm	 Returns the search term.
	//doc GooglePage setSearchTerm(aSeq) Sets the search term. Returns self.
	
    find := method(
		//doc GooglePage find Runs the search.
		u := "http://www.google.com/search?q=" .. searchTerm percentEncoded .. "&num=100&hl=en&lr=&safe=off&start=" .. (pageNumber * 100) .. "&sa=N"
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
/*metadoc GoogleSearch description 
Object for performing web searches via Google.
Example:
<pre>
GoogleSearch clone setSearchTerm("iolanguage") find links foreach(println)
while(Coroutine yieldingCoros size > 1, yield)
</pre>
*/

    links ::= nil
	//doc GoogleSearch links Returns the list of link results.

	maxPages ::= 3
	//doc GoogleSearch maxPages Returns the max number of pages to fetch results from.
	//doc GoogleSearch setMaxPages(aNumber) Sets the max number of pages to fetch results from. Returns self.
	
	searchTerm ::= ""
	//doc GooglePage searchTerm	 Returns the search term.
	//doc GooglePage setSearchTerm(aSeq) Sets the search term. Returns self.

    find := method(
		//doc GoogleSearch find Runs the search.
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



