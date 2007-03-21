Path := Object clone do(
	docCategory("FileSystem")

	hasDriveLetters := System platform containsAnyCaseSeq("Windows") or System platform containsAnyCaseSeq("Cygwin")

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
        
)

