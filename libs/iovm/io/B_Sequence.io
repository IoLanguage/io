
Vector := Sequence clone setItemType("float32") setEncoding("number")
vector := method(v := Vector clone; call evalArgs foreach(n, v append(n)); v)

Lobby Protos Core do(
	ImmutableSequence := ""
	String := ImmutableSequence
)

Sequence do(
	setSlot("..", method(arg, self asString cloneAppendSeq(arg asString)))

    docSlot("repeated(n)", "Returns a new sequence containing the receiver repeated n number of times.")
    repeated := method(n,
        s := Sequence clone
        n repeat(s appendSeq(self))
        s 
    )

    docSlot("alignLeftInPlace(width, [padding])", "Same as align left but operation is performed on the receiver.")
    alignLeftInPlace := method(width, padding,
        originalSize := size
        padding = padding ifNilEval(" ")
        ((width - size) / padding size) ceil repeat(appendSeq(padding))
        setSize(width max(originalSize))
    )

    docSlot("alignLeft(width, [padding])", """
    Example:
    <pre>
    Io> "abc" alignLeft(10, "-")
    ==> abc-------
    Io> "abc" alignLeft(10, "-=")
    ==> abc-=-=-=-
    </pre>
    """)
    alignLeft := method(width, padding, asMutable alignLeftInPlace(width, padding))

    docSlot("alignRight(width, [padding])", """
    Example:
    <pre>
    Io> "abc" alignRight(10, "-")
    ==> -------abc
    Io> "abc" alignRight(10, "-=")
    ==> -=-=-=-abc
    </pre>
    """)
    alignRight := method(width, padding,
        Sequence clone alignLeftInPlace(width - size, padding) appendSeq(self)
    )

    docSlot("alignCenter(width, [padding])", """
    Example:
    <pre>
    Io> "abc" alignCenter(10, "-")
    ==> ---abc----
    Io> "abc" alignCenter(10, "-=")
    ==> -=-abc-=-=
    </pre>
    """)
    alignCenter := method(width, padding,
        alignRight(((size + width) / 2) floor, padding) alignLeftInPlace(width, padding)
    )
    
    asSimpleString := method("\"" .. self asString .. "\"")

	docSlot("split(optionalArg1, optionalArg2, ...)", """
		Returns a list containing the non-empty sub-sequences of the receiver divided by the given arguments.
		If no arguments are given the sequence is split on white space.
		Examples:
		<pre>
		"a   b  c d" splitNoEmpties => list("a", "b", "c", "d")
		"a***b**c*d" splitNoEmpties("*") => list("a", "b", "c", "d")
		"a***b||c,d" splitNoEmpties("*", "|", ",") => list("a", "b", "c", "d")
		</pre>
		""")
		
	splitNoEmpties := method(
		self performWithArgList("split", call evalArgs) selectInPlace(size != 0)
	)

  docSlot("findNthSeq(aSequence, n)", "Returns a number with the nth occurence of aSequence")
  findNthSeq := method(str, n,
    num := self findSeq(str)
    if(num isNil, return nil)
    if(n == 1, return num)
    num + self slice(num + 1, self size) findNthSeq(str, n - 1)
  ) 

	
	orderedSplit := method(
            separators := call evalArgs
            if(separators size == 0, return list(self))
            i := 0
            skipped := 0
            r := list
            separators foreach(separator,
                j := findSeq(separator, i) ifNil(
                    skipped = skipped + 1
                    continue
                )
                r append(slice(i, j))
                if(skipped > 0,
                    skipped repeat(r append(nil))
                    skipped = 0
                )
                i = j + separator size
            )
            if(size == 0,
                r append(nil)
            ,
                r append(slice(i))
            )
            skipped repeat(r append(nil))
            r
	)

	docSlot("prependSeq(object1, object2, ...)", "Prepends given objects asString in reverse order to the receiver.  Returns self.")
	prependSeq := method(self atInsertSeq(0, call evalArgs join); self)

	sequenceSets := Map clone do(
		atPut("lowercaseSequence",
			lst := list
			"abcdefghijklmnopqrstuvwxyz" foreach(v, lst append(v))
			lst
		)
		atPut("uppercaseSequence",
			lst := list
			at("lowercaseSequence") foreach(v, lst append(v asUppercase))
			lst
		)
		atPut("digitSequence",
			lst := list
			"0123456789" foreach(v, lst append(v))
			lst
		)
		removeSlot("lst")
		removeSlot("v")
	)

	docSlot("asHex", "Returns a hex string for the receiving sequence, e.g., \"abc\" asHex -> \"616263\".")
	asHex := method(
		r := Sequence clone
		self foreach(c, r appendSeq(c asHex alignRight(2, "00")))
		r
	)
)
