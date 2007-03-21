
DocsExtractor := Object clone do(
	type := "DocsExtractor"
    init := method(
		self folder := Directory clone
		self outFile := File clone
    )
	
	setPath := method(path,
		folder setPath(Path with(path, "source"))
		l := launchPath asMutable clipAfterStartOfSeq("/../../")
		try(Lobby doString(l lastPathComponent))
		outFile setPath(Path with(path, "io/zzz_docs.io"))
	)
	
	clean := method(
		outFile remove
	)

    extract := method(
		writeln("extracting io comment code from:")
		outFile remove open 
		sourceFiles foreach(file,
			writeln("  ", file name, " ")
			slices := file contents slicesBetween("/*#io", "*/")
			code := slices join("\n") .. "\n)\n"
			if (code containsSeq("("), 
			
				Lobby doString(code)
				/*
				e := try(Lobby doString(code))
				e ifNonNil(
					writeln("Error: in docs strings of ", file name)
					System exit
				)
				*/
				outFile write(code)
			)
		)
		outFile close
    )

    sourceFiles := method(
		folder files select(file,
			file name beginsWithSeq("Io") and(
			file name containsSeq("Init") not) and(
			file name pathExtension == "c")
		)
    )
)

de := DocsExtractor clone
de setPath(args at(1))
if(args at(3) == "clean", de clean, de extract)



