
SequenceTest := UnitTest clone do(
	plat := System platform
	isOnWindows := plat beginsWithSeq("Windows") or plat beginsWithSeq("mingw")

	testClone := method(
		string := "blah"
		assertSame(string, string clone)
	)

	testSymbol := method(
		foundImmutableMutable := nil
		for(i, 1, 10,
			for(j, 1, 10,
				a := j asString asMutable
				if (a isSymbol, foundImmutableMutable := 1; break)
				doString("i + j")
			)
			Collector collect
		)
		assertNil(foundImmutableMutable)
	)

	testAsNumber := method(
		assertEquals(5, "5" asNumber)
		assertEquals(123, "  123  " asNumber)
		assertEquals(1, "1 2" asNumber)
		assertEquals(1.2, "1.2" asNumber)
		//assertEquals(Number constants nan, "five" asNumber)
		// Windows systems print out NaN constants differently depending on class
		if(isOnWindows,
			assertEquals(Number constants nan asString, "-1.#IND") // 0/0
			assertEquals("" asNumber asString, "1.#QNAN") // Explicit quiet NaN
		,
			assertEquals(Number constants nan asString, "" asNumber asString)
		)
	)

	testAsString := method(
		string := "blah"
		assertSame(string, string asString)
	)

	testAsList := method(
		assertEquals(list("a", "b", "c", "d"), "abcd" asList)
		vector := Sequence clone setItemType("float32") setEncoding("number")
		vector atPut(0, -0.5)
		vector atPut(1, 0)
		vector atPut(2, 0.5)
		assertEquals(list(-0.5, 0, 0.5), vector asList)
	)

	testAsMutableSequence := method(
		string := "blah"
		assertEquals(Sequence type, string asMutable type)
		assertEquals(string, string asMutable asString)
	)

	testAt := method(
		assertRaisesException("abc" at(nil))
		assertEquals(97, "abc" at(0))
		assertEquals(99, "abc" at(2))
		assertEquals(nil, "abc" at(3))
		assertRaisesException("abc" at(-1))
		assertRaisesException("abc" at(-3))
		assertRaisesException("abc" at(-4))
	)

	testFindSeq := method(
		assertRaisesException("abcd" findSeq(nil))
		assertNil("abcd" findSeq("e"))
		assertNil("abcd" findSeq("abcde"))
		assertEquals(0, "abcabc" findSeq("a"))
		assertEquals(0, "abcabc" findSeq("abc"))
		assertEquals(1, "abcabc" findSeq("bc"))
		assertEquals(3, "abcabc" findSeq("a", 1))
		assertEquals(3, "abcabc" findSeq("a", 3))
		assertNil("abcabc" findSeq("a", 4))
	)

	testReverseFindSeq := method(
		assertRaisesException("abcd" reverseFindSeq(nil))
		assertNil("abcd" reverseFindSeq("e"))
		assertNil("abcd" reverseFindSeq("abcde"))
		assertEquals(3, "abcabc" reverseFindSeq("a"))
		assertEquals(3, "abcabc" reverseFindSeq("abc"))
		assertEquals(4, "abcabc" reverseFindSeq("bc"))
		assertEquals(0, "abcabc" reverseFindSeq("a", 1))
		assertNil("abcabc" reverseFindSeq("a", 0))
		assertEquals(3, "abcabc" reverseFindSeq("a", 4))
	)

	testbeginsWithSeq := method(
		assertRaisesException("abcd" beginsWithSeq(nil))
		assertFalse("abcd" beginsWithSeq("zz"))
		assertFalse("abcd" beginsWithSeq("b"))
		assertTrue("abcd" beginsWithSeq("a"))
		assertTrue("abcd" beginsWithSeq("abcd"))
	)

	testendsWithSeq := method(
		assertRaisesException("abcd" endsWithSeq(nil))
		assertFalse("abcd" endsWithSeq("zz"))
		assertFalse("abcd" endsWithSeq("b"))
		assertTrue("abcd" endsWithSeq("d"))
		assertTrue("abcd" endsWithSeq("abcd"))
	)

	testRemovePrefix := method(
		assertRaisesException("abc" asMutable removePrefix(nil))
		assertEquals("bc", "abc" asMutable removePrefix("a"))
		assertEquals("abc", "abc" asMutable removePrefix("b"))
		assertEquals("", "abc" asMutable removePrefix("abc"))
	)

	testRemoveSuffix := method(
		assertRaisesException("abc" asMutable asMutable removeSuffix(nil))
		assertEquals("ab", "abc" asMutable asMutable removeSuffix("c"))
		assertEquals("abc", "abc" asMutable asMutable removeSuffix("b"))
		assertEquals("", "abc" asMutable asMutable removeSuffix("abc"))
	)

	testContainsSeq := method(
		assertRaisesException("abcd" containsSeq(nil))
		assertFalse("abcd" containsSeq("zz"))
		assertFalse("abcd" containsSeq("D"))
		assertTrue("abcd" containsSeq("d"))
		assertTrue("abcd" containsSeq("abcd"))
	)

	testContainsAnyCaseSeq := method(
		assertRaisesException("abcd" containsAnyCaseSeq(nil))
		assertFalse("abcd" containsAnyCaseSeq("zz"))
		assertTrue("abcd" containsAnyCaseSeq("d"))
		assertTrue("abcd" containsAnyCaseSeq("D"))
		assertTrue("ABCD" containsAnyCaseSeq("AbCd"))
	)

	testLength := method(
		assertEquals(0, "" size)
		assertEquals(3, "abc" size)
	)

	testSplit := method(
		assertRaisesException("abc" splitNoEmpties(nil))
		assertEquals(List clone append("abc"), "abc" splitNoEmpties("d"))
		assertEquals(List clone append("this", "is", "a", "test"), "this is a \ttest" splitNoEmpties(" ", "\t"))
		assertEquals(List clone append("This ", "is", " a #split# test!"), "This ##is## a #split# test!" splitNoEmpties("##"))
	)

	testSplitAt := method(
		assertRaisesException("abc" splitAt(nil))
		assertEquals(List clone append("a", "bc"), "abc" splitAt(1))
		assertEquals(List clone append("", "abc"), "abc" splitAt(0))
		assertEquals(List clone append("abc", ""), "abc" splitAt(3))
		assertEquals(List clone append("abc", ""), "abc" splitAt(55))
	)

	testStrip := method(
		assertRaisesException("abc" asMutable strip(nil))
		assertEquals("abc", "   abc" asMutable lstrip)
		assertEquals(" abc", " abc " asMutable rstrip)
		assertEquals("abc", " abc " asMutable strip)
		assertEquals("abc", "	abc" asMutable strip("	"))
		assertEquals("bc", "   abc" asMutable strip("a "))
		assertEquals("ab", "   abc" asMutable strip("c "))
		assertEquals("", "  \r\t\n  " asMutable strip)
		assertEquals("", "  \r\t\n  " asMutable lstrip)
		assertEquals("", "  \r\t\n  " asMutable rstrip)
	)

	testRStrip := method(
		assertRaisesException("abc" asMutable rstrip(nil))
		assertEquals("abc", "abc   " asMutable rstrip(" "))
		assertEquals("abc", "abc " asMutable rstrip("  "))
		assertEquals("ab", "abc   " asMutable rstrip("c "))
		assertEquals("abc", "abc   " asMutable rstrip("a "))
	)

	testCompare := method(
		assertRaisesException("abc" compare)
		assertTrue( "abc" compare("abc") == 0 )
		assertTrue( "ab"  compare("abc") < 0 )
		assertTrue( "a"   compare("abc") < 0 )
		assertTrue( "abc" compare("ab")  > 0 )
		assertTrue( "abc" compare("a c") > 0 )
	)

	testAppend := method(
		assertRaisesException("abc" appendSeq)
		string := "abc"
		assertNotSame(string, string .. "def")
		assertEquals("abcdef", "abc" .. "def")
	)

	testAtInsert := method(
		assertRaisesException("abc" asMutable atInsertSeq)
		assertRaisesException("abc" asMutable atInsertSeq(nil))
		assertRaisesException("abc" asMutable atInsertSeq(nil, "def"))
		assertEquals("a123bc", "abc" asMutable atInsertSeq(1, 123))
		assertEquals("anilbc", "abc" asMutable atInsertSeq(1, nil))
		string := "abc"
		assertNotSame(string, string asMutable atInsertSeq(1, "def"))
		assertEquals("abcdef", "abc" asMutable atInsertSeq(3, "def"))
		assertRaisesException("abc" asMutable atInsertSeq(9, "def"))
		assertEquals("abdefc", "abc" asMutable atInsertSeq(2, "def"))
		assertEquals("defabc", "abc" asMutable atInsertSeq(0, "def"))
		assertRaisesException("abc" asMutable atInsertSeq(-8, "def"))
	)

	testRemoveSlice := method(
		assertRaisesException("abc" asMutable removeSlice)
		assertRaisesException("abc" asMutable removeSlice(nil))
		assertRaisesException("abc" asMutable removeSlice(1, nil))
		assertRaisesException("abc" asMutable removeSlice(nil, 2))
		string := "abc"
		assertNotSame(string, string asMutable removeSlice(1, 2))
		assertEquals("ac", "abc" asMutable removeSlice(1, 1))
		assertEquals("a", "abc" asMutable removeSlice(1, 2))
		assertEquals("abc", "abc" asMutable removeSlice(2, 1))
		assertEquals("c", "abc" asMutable removeSlice(0, 1))
		assertEquals("af", "abcdef" asMutable removeSlice(1, 4))
		assertEquals("", "abc" asMutable removeSlice(0, 2))
		assertEquals("a", "abc" asMutable removeSlice(1, 8))
		assertEquals("c", "abc" asMutable removeSlice(-8, 1))
	)

		testRemoveAt := method(
			assertRaisesException("abc" asMutable removeAt)
			assertRaisesException("abc" asMutable removeAt(nil))
			assertEquals("ab", "abc" asMutable removeAt(-1))
			assertEquals("abc", "abc" asMutable removeAt(3))
			assertEquals("bc", "abc" asMutable removeAt(0))
			assertEquals("ac", "abc" asMutable removeAt(1))
			assertEquals("ab", "abc" asMutable removeAt(2))
		)

	testReplaceMap := method(
		assertRaisesException("abc" asMutable replaceMap)
		assertRaisesException("abc" asMutable replaceMap(nil))
		assertRaisesException("abc" asMutable replaceMap(Map clone atPut("a", nil)))
		string := "abc"
		map := Map clone atPut("a", "BB") atPut("c", "CC")
		assertNotSame(string, string asMutable replaceMap(map))
		assertEquals("BBbCC", string asMutable replaceMap(map))
	)

	testReplace := method(
		assertRaisesException("abc" asMutable replaceSeq)
		assertRaisesException("abc" asMutable replaceSeq(nil))
		assertRaisesException("abc" asMutable replaceSeq("a", nil))
		assertRaisesException("abc" asMutable replaceSeq(nil, "AA"))
		string := "abc"
		assertNotSame(string, string asMutable replaceSeq("a", "AA"))
		assertEquals("AAbc", "abc" asMutable replaceSeq("a", "AA"))
		assertEquals("aaaabcaa", "aabca" asMutable replaceSeq("a", "aa"))
	)

	testIsLowercase := method(
		assertTrue("abc" isLowercase)
		assertTrue("" isLowercase)
		assertTrue("a123b " isLowercase)
		assertFalse("aBc" isLowercase)
	)

	testIsUppercase := method(
		assertTrue("ABC" isUppercase)
		assertTrue("" isUppercase)
		assertTrue("A123B " isUppercase)
		assertFalse("AbC" isUppercase)
	)

	testLower := method(
		assertSame("abc", "abc" asLowercase)
		string := "ABC"
		assertNotSame(string, string asLowercase)
		assertEquals("abc", "ABC" asLowercase)
		assertEquals(" abc12 ", " AbC12 " asLowercase)
	)

	testUpper := method(
		assertSame("ABC", "ABC" asUppercase)
		string := "abc"
		assertNotSame(string, string asUppercase)
		assertEquals("ABC", "abc" asUppercase)
		assertEquals(" ABC12 ", " aBc12 " asUppercase)
	)

	testAsCapitalized := method(
		assertSame("Abc", "Abc" asCapitalized)
		string := "abc"
		assertNotSame(string, string asCapitalized)
		assertEquals("Abc", "abc" asCapitalized)
		assertEquals("ABc12 ", "aBc12 " asCapitalized)
		assertEquals(" a b c", " a b c" asCapitalized)
	)

	testIsEqual := method(
		assertRaisesException("abc" ==)
		assertFalse("abc" == nil)
		assertTrue("ABC" == "ABC")
		assertTrue("" == "")
		assertFalse("abc" == "ABC")
		assertFalse("AbC" == "AbCd")
	)

	testIsEqualAnyCase := method(
		assertRaisesException("abc" isEqualAnyCase)
		assertRaisesException("abc" isEqualAnyCase(nil))
		assertTrue("ABC" isEqualAnyCase("abc"))
		assertTrue("" isEqualAnyCase(""))
		assertFalse("AbC" isEqualAnyCase("AbCd"))
	)

	testEscape := method(
		assertEquals("Abc", "Abc" asMutable escape)
		string := "\\\""
		assertNotSame(string, string asMutable escape)
		assertEquals("\\\" \\a \\b \\f \\n \\r \\t \\v \\0 \\\\", "\" \a \b \f \n \r \t \v \0 \\" asMutable escape)
	)

	testUnescape := method(
		assertEquals("Abc", "Abc" asMutable unescape)
		string := "\\\""
		assertNotSame(string, string asMutable unescape)
		assertEquals("\" \a \b \f \n \r \t \v \0 \\", "\\\" \\a \\b \\f \\n \\r \\t \\v \\0 \\\\" asMutable unescape)
	)

	_testPrint := method(
		// would need to be able to read stdout or install a printCallback from Io to test print()
		nil
	)

	_testLinePrint := method(
		// would need to be able to read stdout or install a printCallback from Io to test print()
		nil
	)

	testJoin := method(
		assertRaisesException(String join)
		assertRaisesException(String join(nil))
		assertEquals("abc", list("a", "b", "c") join)
		assertEquals("", ".." list() join)
		assertEquals("a", list("a") join("--"))
		assertEquals("a--b--c", list("a", "b", "c") join("--"))
	)

	_testLeftJustified := method(
		assertRaisesException(String leftJustified)
		assertRaisesException(String asMutable leftJustified)
		assertRaisesException(String asMutable leftJustified(nil))
		assertRaisesException(String asMutable leftJustified(nil, nil))
		assertRaisesException(String asMutable leftJustified(nil, 'a'))
		assertEquals("abc", "abc" asMutable leftJustified(2))
		assertEquals("abc", "abc" asMutable leftJustified(3))
		assertEquals("abc ", "abc" asMutable leftJustified(4))
		assertEquals("abcxx", "abc" asMutable leftJustified(5, "x" at(0)))
	)

	_testRightJustified := method(
		assertRaisesException(String rightJustified)
		assertRaisesException(String asMutable rightJustified)
		assertRaisesException(String asMutable rightJustified(nil))
		assertRaisesException(String asMutable rightJustified(nil, nil))
		assertRaisesException(String asMutable rightJustified(nil, 'a'))
		assertEquals("abc", "abc" asMutable rightJustified(2))
		assertEquals("abc", "abc" asMutable rightJustified(3))
		assertEquals(" abc", "abc" asMutable rightJustified(4))
		assertEquals("11abc", "abc" asMutable rightJustified(5, "1" at(0)))
	)

	_testCentered := method(
		assertRaisesException(String centered)
		assertRaisesException(String asMutable centered(nil))
		assertRaisesException(String asMutable centered(nil, nil))
		assertRaisesException(String asMutable centered(nil, 'a'))
		assertEquals("abc", "abc" asMutable centered(2))
		assertEquals("abc", "abc" asMutable centered(3))
		assertEquals("abc ", "abc" asMutable centered(4))
		assertEquals(" abc ", "abc" asMutable centered(5))
		assertEquals("-abc--", "abc" asMutable centered(6, "-" at(0)))
	)

	testFromBase := method(
		assertRaisesException("1f" fromBase)
		assertRaisesException("1f" fromBase(nil))
		assertRaisesException("g" fromBase(16))
		assertRaisesException("9" fromBase(8))
		assertEquals(15*16 + 15, "ff" fromBase(16))
		assertEquals(7*8*8 + 1, "701" fromBase(8))
	)

	testToBase := method(
		assertRaisesException("12" toBase)
		assertRaisesException("12" toBase(nil))
		assertEquals("0", "a" toBase(16))
		assertEquals("0", "a" toBase(8))
		assertEquals("ff", "255" toBase(16))
		assertEquals("701", "449" toBase(8))
	)

	testAppendPath := method(
		string := "abc"
		assertNotSame(string, Path with(string, "def"))

		assertEquals("abc/def", Path with("abc/", "def"))
		assertEquals("abc/def/", Path with("abc", "/def/"))
		assertEquals("/abc/def/", Path with("/abc/", "/def/"))

		if(isOnWindows,
			assertEquals("abc\\def", Path with("abc\\", "def"))
			assertEquals("abc\\def\\", Path with("abc", "\\def\\"))
			assertEquals("\\abc\\def\\", Path with("\\abc\\", "\\def\\"))
			assertEquals("abc\\def", Path with("abc", "def"))
		,
			assertEquals("abc/def", Path with("abc", "def"))
		)
	)

	testPathComponent := method(
		string := "abc"
		assertNotSame(string, string pathComponent)

		assertEquals("abc", "abc/def" pathComponent)
		assertEquals("abc", "abc/def/" pathComponent)
		assertEquals("abc", "abc/def//" pathComponent)
		assertEquals("", "abc/" pathComponent)

		if(isOnWindows,
			assertEquals("abc", "abc\\def" pathComponent)
			assertEquals("abc", "abc\\def\\" pathComponent)
			assertEquals("abc", "abc\\def\\\\" pathComponent)
			assertEquals("", "abc\\" pathComponent)
		)
	)

	testLastPathComponent := method(
		assertEquals("abc", "abc" lastPathComponent)

		assertEquals("def", "abc/def" lastPathComponent)
		assertEquals("def/", "abc/def/" lastPathComponent)
		assertEquals("def//", "abc/def//" lastPathComponent)

		if(isOnWindows,
			assertEquals("def", "abc\\def" lastPathComponent)
			assertEquals("def\\", "abc\\def\\" lastPathComponent)
			assertEquals("def\\\\", "abc\\def\\\\" lastPathComponent)
		)
	)

	testPathExtension := method(
		assertEquals("", "" pathExtension)
		assertEquals("", "abc" pathExtension)
		assertEquals("c", "./.a.b.c" pathExtension)
	)

	testFileName := method(
		assertSame("", "" fileName)
		assertEquals("abc", "abc" fileName)

		assertEquals(".a.b", "c./.a.b.c" fileName)

		if(isOnWindows,
			assertEquals(".a.b", "c.\\.a.b.c" fileName)
		)
	)

	testForeach := method(
		assertRaisesException("a" foreach)
		assertRaisesException("a" foreach(index))
		assertRaisesException("a" foreach(index, value))
		"a" foreach(index, value, nil)
		"" foreach(index, value, nil)
		total := 0
		totalIndex := 0
		"abc" foreach(index, value, total = total + value; totalIndex = totalIndex + index)
		assertEquals("a" at(0) + "b" at(0) + "c" at(0), total)
		assertEquals(0 + 1 + 2, totalIndex)
	)

	testForeachSingleArg := method(
		Lobby values := list
		Number addToValuesList := method(
			values append(self)
		)
		"abc" foreach(addToValuesList)

		assertEquals(values, list(97, 98, 99))

		Lobby removeSlot("values")
		Number removeSlot("addToValuesList")
	)

	testAsMessage := method(
		string := "blah"
		assertEquals(Message type, string asMessage type)
		assertEquals(string, string asMessage asString)
	)

	testAfter := method(
		assertRaisesException( "" afterSeq )
		assertRaisesException( "" afterSeq(nil) )
		assertEquals(nil,  "" afterSeq(""))
		assertEquals("a",  "a" afterSeq(""))
		assertEquals("",  "a" afterSeq("a"))
		assertEquals("b",  "ab" afterSeq("a"))
		assertEquals("ba",  "aba" afterSeq("a"))
		assertEquals(nil,  "aba" afterSeq("c"))
	)

	testBefore := method(
		assertRaisesException( "" beforeSeq )
		assertRaisesException( "" beforeSeq(nil) )
		assertEquals("",  "" beforeSeq(""))
		assertEquals("",  "a" beforeSeq(""))
		assertEquals("",  "a" beforeSeq("a"))
		assertEquals("a",  "ab" beforeSeq("b"))
		assertEquals("a",  "abcbd" beforeSeq("b"))
		assertEquals("aba",  "aba" beforeSeq("c"))
	)

	testExclusiveSlice := method(
		assertRaisesException("" exclusiveSlice)

		assertEquals("", "" exclusiveSlice(1))
		assertEquals("bcdef", "abcdef" exclusiveSlice(1))
		assertEquals("f", "abcdef" exclusiveSlice(-1))
		assertEquals("", "abcdef" exclusiveSlice(20))
		assertEquals("abcdef", "abcdef" exclusiveSlice(-20))

		assertEquals("", "" exclusiveSlice(1,4))

		assertEquals("b", "abcdef" exclusiveSlice(1,2))
		assertEquals("e", "abcdef" exclusiveSlice(-2,-1))

		assertEquals("", "abcdef" exclusiveSlice(2,1))
		assertEquals("", "abcdef" exclusiveSlice(-1,-2))

		assertEquals("bcde", "abcdef" exclusiveSlice(1,-1))
		assertEquals("", "abcdef" exclusiveSlice(-1,1))
		assertEquals("", "abcdef" exclusiveSlice(3,-3))
		assertEquals("c", "abcdef" exclusiveSlice(2,-3))
		assertEquals("cdef", "abcdef" exclusiveSlice(2,2000))
		assertEquals("", "abcdef" exclusiveSlice(2,-2000))
	)
	
	testInclusiveSlice := method(
		assertRaisesException("" inclusiveSlice)

		assertEquals("", "" inclusiveSlice(1))
		assertEquals("bcdef", "abcdef" inclusiveSlice(1))
		assertEquals("f", "abcdef" inclusiveSlice(-1))
		assertEquals("", "abcdef" inclusiveSlice(20))
		assertEquals("abcdef", "abcdef" inclusiveSlice(-20))

		assertEquals("", "" inclusiveSlice(1,4))

		assertEquals("bc", "abcdef" inclusiveSlice(1,2))
		assertEquals("ef", "abcdef" inclusiveSlice(-2,-1))

		assertEquals("", "abcdef" inclusiveSlice(2,1))
		assertEquals("", "abcdef" inclusiveSlice(-1,-2))

		assertEquals("bcdef", "abcdef" inclusiveSlice(1,-1))
		assertEquals("", "abcdef" inclusiveSlice(-1,1))
		assertEquals("d", "abcdef" inclusiveSlice(3,-3))
		assertEquals("cd", "abcdef" inclusiveSlice(2,-3))
		assertEquals("cdef", "abcdef" inclusiveSlice(2,2000))
		assertEquals("", "abcdef" inclusiveSlice(2,-2000))
	)
	
	testBetweenSeq := method(
	  assertEquals("bc", "abcd" betweenSeq("a", "d"))
	  assertEquals(nil, "bcd" betweenSeq("a", "d"))
	  assertEquals(nil, "abc" betweenSeq("a", "d"))
	  assertEquals(nil, "" betweenSeq("aaa", "bbb"))	  
	)
	
	testSetSize := method(
		assertRaisesException(Sequence clone setSize(-1))
		assertRaisesException(Sequence clone setSize(-10))
	)

	testPreallocateToSize := method(
		assertRaisesException(Sequence clone preallocateToSize(-1))
		assertRaisesException(Sequence clone preallocateToSize(-10))
	)

	testInterpolate := method(
		assertEquals("" interpolate type, ImmutableSequence type)

		assertEquals("", "" interpolate)
		assertEquals("", "#{}" interpolate)

		assertEquals("abcd", "#{}abcd" interpolate)
		assertEquals("abcd", "abcd#{}" interpolate)
		assertEquals("abcd", "ab#{}cd" interpolate)

		assertEquals("8abcd", "#{1+7}abcd" interpolate)
		assertEquals("abcd8", "abcd#{1+7}" interpolate)
		assertEquals("ab8cd", "ab#{1+7}cd" interpolate)

		ctx := Object clone do(
			a := 1
			b := 7
		)
		assertEquals("8abcd", "#{a+b}abcd" interpolate(ctx))
		assertEquals("abcd8", "abcd#{a+b}" interpolate(ctx))
		assertEquals("ab8cd", "ab#{a+b}cd" interpolate(ctx))
	)

	testInterpolateInPlace := method(
		assertEquals("" asMutable interpolateInPlace type, Sequence type)

		assertEquals("", "" asMutable interpolateInPlace)
		assertEquals("", "#{}" asMutable interpolateInPlace)

		assertEquals("abcd", "#{}abcd" asMutable interpolateInPlace)
		assertEquals("abcd", "abcd#{}" asMutable interpolateInPlace)
		assertEquals("abcd", "ab#{}cd" asMutable interpolateInPlace)

		ctx := Object clone do(
			a := 1
			b := 7
		)
		assertEquals("8abcd", "#{1+7}abcd" asMutable interpolateInPlace(ctx))
		assertEquals("abcd8", "abcd#{1+7}" asMutable interpolateInPlace(ctx))
		assertEquals("ab8cd", "ab#{1+7}cd" asMutable interpolateInPlace(ctx))
	)

	testBadInterpolateInPlace := method(
		try(
			x := Object clone do( asString := nil )
			"#{x}" interpolate
			y := Object clone removeAllProtos
			"#{y}" interpolate
		)
	)

	_testNextInSequence := method(
		assertEquals("b", "a" nextInSequence)
		assertEquals("aa", "z" nextInSequence)

		assertEquals("ab", "aa" nextInSequence)
		assertEquals("ba", "az" nextInSequence)

		assertEquals("B", "A" nextInSequence)
		assertEquals("AA", "Z" nextInSequence)

		assertEquals("AB", "AA" nextInSequence)
		assertEquals("BA", "AZ" nextInSequence)

		assertEquals("aB", "aA" nextInSequence)
		assertEquals("Ab", "Aa" nextInSequence)

		assertEquals("bA", "aZ" nextInSequence)
		assertEquals("Ba", "Az" nextInSequence)

		assertEquals("1", "0" nextInSequence)
		assertEquals("10", "9" nextInSequence)

		assertEquals("<<abd>>", "<<abc>>" nextInSequence)
		assertEquals("<<ba>>", "<<az>>" nextInSequence)
		assertEquals("<<10>>", "<<9>>" nextInSequence)
		assertEquals("<<>>", "<<>>" nextInSequence)

		assertEquals("f", "a" nextInSequence(5))
		assertEquals("ae", "z" nextInSequence(5))
	)

	testReverseInPlace := method(
		s := "" asMutable
		s reverseInPlace
		assertEquals("" asMutable, s)
		
		s := "a" asMutable
		s reverseInPlace
		assertEquals("a" asMutable, s)
		
		s := "cba" asMutable
		s reverseInPlace
		assertEquals("abc" asMutable, s)
		assertRaisesException("abc" reverseInPlace)
	)
	
	testReverse := method(
		a := ""
		b := a reverse
		assertEquals("" asMutable, b)
		assertNotSame(a, b)
		
		a := "a"
		b := a reverse
		assertEquals("a" asMutable, b)
		assertNotSame(a, b)
		
		a := "cba"
		b := a reverse
		assertEquals("abc" asMutable, b)
		assertNotSame(a, b)
	)

	testPrependSeq := method(
		assertEquals("", "" asMutable prependSeq(""))
		assertEquals("a", "" asMutable prependSeq("a"))
		assertEquals("1a", "a" asMutable prependSeq("1"))
		assertEquals("123", "" asMutable prependSeq("123"))
		assertEquals("123a", "a" asMutable prependSeq("123"))
		assertEquals("123a", "a" asMutable prependSeq("1", "2", "3"))
		assertEquals("123", "" asMutable prependSeq("1", "2", "3"))
	)

	testCopy := method(
		assertRaisesException("abc" copy("123"))
		assertEquals("123", "abc" asMutable copy("123"))
		assertEquals("123", "abc" asMutable do(copy("123")))
	)
	
	testInsertSeqEvery := method(
		assertRaisesException("abc" insertSeqEvery(".", 1))
		assertRaisesException("abc" asMutable insertSeqEvery(".", 0))
		assertRaisesException("abc" asMutable insertSeqEvery(".", 4))
		assertEquals("a.b.c.", "abc" asMutable insertSeqEvery(".", 1))
		assertEquals("a..b..c..", "abc" asMutable insertSeqEvery("..", 1))
		assertEquals("ab.c", "abc" asMutable insertSeqEvery(".", 2))
		assertEquals("abc.", "abc" asMutable insertSeqEvery(".", 3))
	)
	
	testLeaveThenRemove := method(
		assertRaisesException("abc" leaveThenRemove(1, 1))
		assertRaisesException("abc" asMutable leaveThenRemove(0, 0))
		assertEquals("", "abc" asMutable leaveThenRemove(0, 1))
		assertEquals("abc", "abc" asMutable leaveThenRemove(1, 0))
		assertEquals("ac", "abc" asMutable leaveThenRemove(1, 1))
		assertEquals("ad", "abcd" asMutable leaveThenRemove(1, 2))
		assertEquals("a", "abcd" asMutable leaveThenRemove(1, 4))
		assertEquals("abe", "abcde" asMutable leaveThenRemove(2, 2))
	)
)
