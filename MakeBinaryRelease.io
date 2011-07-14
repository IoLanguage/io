dynlibs := Directory with("addons") directories map(f,
	r := nil
	try(
		r = f at("_build") at("dll") files select(name endsWithSeq(".dylib")) first
	)
	r
) select(!= nil)

depends := dynlibs map(dllFile,
   lines := SystemCall clone setCommand("otool") setArguments(list("-L", dllFile path)) run stdout readLines 
	//foreach(println)
	lines mapInPlace(beforeSeq("(") strip) 
	lines removeFirst
	lines selectInPlace(beginsWithSeq("/"))
	lines selectInPlace(beginsWithSeq("/usr/lib/") not)
	lines selectInPlace(beginsWithSeq("/usr/local/lib/io/addons/") not)
	lines selectInPlace(beginsWithSeq("/System/Library/") not)
	if(lines isEmpty not, 
		writeln("\n", dllFile name, " needs: ")
		lines foreach(line, writeln("	", line))
	)
	lines
) flatten

external := Directory directoryNamed("_build") createSubdirectory("external")

installScript := external fileNamed("install.sh") remove open

depends foreach(depend,
	writeln("copying ", depend, " to ", external path)
	installScript write("cp ", Path with(external path, depend lastPathComponent), " ", depend, "\n");
	SystemCall clone setCommand("cp") setArguments(list(depend, external path)) run stdout readLines 
)
installScript close
SystemCall clone setCommand("chmod") setArguments(list("ug+x", installScript path)) run 

/*
/opt/local/include/ncurses

*/