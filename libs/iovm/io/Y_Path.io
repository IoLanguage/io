Path := Object clone do(
	docCategory("FileSystem")

	docSlot("hasDriveLetters", "returns true if the platform requires DOS C: style drive letters.")
	hasDriveLetters := System platform containsAnyCaseSeq("Windows") or System platform containsAnyCaseSeq("Cygwin")

	docSlot("with(aSequence)", "Returns a new Path object for the given Sequence.")
	with := method(
		s := Sequence clone
		call message arguments foreach(arg,
			v := call sender doMessage(arg)
			//writeln("appendPathSeq(", v type, ")")
			if(v == nil, v = "")
			s appendPathSeq(v)
		)
		s asSymbol
	)

	docSlot("isPathAbsolute", "Returns true if path is absolute, false if it is relative.")
	isPathAbsolute := method(p,
		absolute := false
		try (
			if (hasDriveLetters,
				absolute = p at(0) isLetter and p at(1) asCharacter == ":" or p at(0) asCharacter == "/" or p at(0) asCharacter == "\\"
			,
				absolute = p at(0) asCharacter == "/"
			)
		)
		absolute
	)

	docSlot("absolute", "Returns an absolute version of the path.")
	absolute := method(path,
		if(isPathAbsolute(path),
			path
		,
			with(Directory currentWorkingDirectory, path)
		)
	)
)

