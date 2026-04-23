
File do(
	// Normalize "// metadoc Foo" → "//metadoc Foo" so slicesBetween matches
	// both forms. The codebase mixes the two (with and without the space after //).
	normalizedContents := method(
		contents asMutable replaceSeq("// metadoc ", "//metadoc ") replaceSeq("// cmetadoc ", "//cmetadoc ") replaceSeq("// doc ", "//doc ") replaceSeq("// cdoc ", "//cdoc ")
	)

	docSlicesFor := method(name,
		c := normalizedContents
		c slicesBetween("//" .. name .. " ", "\n") map(strip) map(s,
			i := s findSeq(" ") + 1 // after doc
			i1 := s findSeq("(", i)  // (
			i2 := s findSeq(" ", i) //
			if(i1 and i1 < i2, i2 := s findSeq(")", i))
			if(i2, s atInsertSeq(i2 + 1, "\n"))
			s
		) appendSeq(c slicesBetween("/*" .. name .. " ", "*/"))
	)

	docSlices := method(
		docSlicesFor("doc")
	)

	cdocSlices := method(
		docSlicesFor("cdoc")
	)

	metadocSlices := method(
		docSlicesFor("metadoc")
	)

	cmetadocSlices := method(
		docSlicesFor("cmetadoc")
	)
)

DocsExtractor := Object clone do(
	init := method(
		self folder := Directory clone
		self outFile := File clone
		self cOutFile := File clone
	)

	setPath := method(path,
		folder setPath(path) createSubdirectory("docs")
		// Io-visible API docs (/*doc ...*/, /*metadoc ...*/)
		outFile setPath(Path with(path, "docs/docs.txt"))
		// C-internal implementation docs (/*cdoc ...*/)
		cOutFile setPath(Path with(path, "docs/cdocs.txt"))
		self
	)

	clean := method(
		outFile remove
		cOutFile remove
	)

	extract := method(
		outFile remove open
		cOutFile remove open
		sourceFiles foreach(file,
			file docSlices foreach(d,
				outFile write("doc ", d strip, "\n------\n")
			)
			file metadocSlices foreach(d,
				outFile write("metadoc ", d strip, "\n------\n")
			)
			file cdocSlices foreach(d,
				cOutFile write("cdoc ", d strip, "\n------\n")
			)
			file cmetadocSlices foreach(d,
				cOutFile write("cmetadoc ", d strip, "\n------\n")
			)
		)
		outFile close
		cOutFile close
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

// docs2html.io loads this file as a library via doRelativeFile and
// drives extraction itself. No top-level side effects here.
