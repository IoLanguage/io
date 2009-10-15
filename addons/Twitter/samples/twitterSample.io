//TwitterRequest debugOn
TwitterSearch clone setQuery("richcollins") setPerPage(10) setPage(1) results ifError(e, writeln(e message)) foreach(result,
	writeln(result at("from_user"), ": ", result at("text"))
)