//TwitterRequest debugOn
c := TwitterSearch clone setQuery("richcollins") setPerPage(10) setPage(1) cursor
i := 0
while(c next,
	writeln(c result at("text"))
	i = i + 1
)
writeln(i)