
File do(
	docSlicesFor := method(name,
		contents slicesBetween("//" .. name .. " ", "\n") map(strip) map(s,
			i := s findSeq(" ") + 1 // after doc
			i1 := s findSeq("(", i)  // (
			i2 := s findSeq(" ", i) // 
			if(i1 and i1 < i2, i2 := s findSeq(")", i))
			if(i2, s atInsertSeq(i2 + 1, "\n"))
			s
		) appendSeq(contents slicesBetween("/*" .. name .. " ", "*/"))		
	)
	
	docSlices := method(
		docSlicesFor("doc")
	)
	
	metadocSlices := method(
		docSlicesFor("metadoc")
	)
)

DocsExtractor := Object clone do(
	init := method(
		self folder := Directory clone
		self outFile := File clone
	)

	setPath := method(path,
		folder setPath(path) createSubdirectory("docs")
		outFile setPath(Path with(path, "docs/docs.txt"))
	)

	clean := method(
		outFile remove
	)

	extract := method(
		//writeln("\n", folder path)
		outFile remove open
		sourceFiles foreach(file,
			//writeln("	", file name, " ")
			file docSlices foreach(d,
				/*
				header := d beforeSeq("\n") strip
				protoName := header beforeSeq(" ")
				slotName := header afterSeq(" ")
				comment := d afterSeq("\n")
				*/
				outFile write("doc ", d strip, "\n------\n")
			)
			
			file metadocSlices foreach(d,
				outFile write("metadoc ", d strip, "\n------\n")
			)
		)
		outFile close
	)
	
	sourceFiles := method(cFiles appendSeq(ioFiles))
	//sourceFiles := method(ioFiles)

	cFiles := method(
		if(folder directoryNamed("source") exists,
			folder directoryNamed("source") files select(file,
				file name beginsWithSeq("Io") and(
				file name containsSeq("Init") not) and(
				file name pathExtension == "c" or file name pathExtension == "m")
			)
		,
			list()
		)
	)
	
	ioFiles := method(
		if(folder directoryNamed("io") exists, folder directoryNamed("io") filesWithExtension("io"), list())
	)
)

de := DocsExtractor clone
de setPath(System args at(1))
if(System args at(3) == "clean", de clean, de extract)
