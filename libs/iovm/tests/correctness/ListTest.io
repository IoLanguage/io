
ListTest := UnitTest clone do(
	setUp := method(
		super(setUp)
		self exampleList := List clone append("a", "beta", 3)
	)
	testClone := method(
		assertNotSame(List, List clone)
		clonedList := exampleList clone
		assertNotSame(clonedList, exampleList)
		assertEquals(3, clonedList size)
		assertEquals(exampleList at(0), clonedList at(0))
		assertEquals(exampleList at(2), clonedList at(2))
	)
	testAppend := method(
		assertRaisesException(a append)
		a := List clone append("a", "beta", 3)
		a append(nil)
		a append(88)
		a append("blah", "fasel")
		assertEquals("fasel", a at(6))
		assertEquals(88, a at(4))
		assertNil(a at(3))
	)
	testAppendIfAbsent := method(
		a := List clone append("a", "beta", 3)
		a appendIfAbsent
		a appendIfAbsent(nil)
		a appendIfAbsent(3)
		a appendIfAbsent(3)
		assertEquals(4, a size)
		assertNil(a at(3))
		a appendIfAbsent("blah", "blah", 3, "new1", "blah", "new2")
		assertEquals(7, a size)
		assertEquals("blah", a at(4))
		assertEquals("new1", a at(5))
		assertEquals("new2", a at(6))
	)
	testAppendSeq := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a appendSeq(nil))
		assertRaisesException(a appendSeq(a))
		a appendSeq(List clone)
		a appendSeq(List clone append("blah", "fasel"))
		assertEquals(5, a size)
		assertEquals("blah", a at(3))
		assertEquals("fasel", a at(4))
	)
	testRemove := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a remove)
		a append(nil, 7, nil)
		a remove(nil)
		assertEquals(4, a size)
		a remove(3)
		assertEquals(3, a size)
		assertEquals("a", a at(0))
		assertEquals("beta", a at(1))
		assertEquals(7, a at(2))
	)
	testIndexOf := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a indexOf)
		assertNil(a indexOf(nil))
		a append("a")
		assertEquals(0, a indexOf("a"))
		a removeAt(0)
		assertEquals(2, a indexOf("a"))
	)
	testContains := method(
		a := List clone append("a", "beta", 3)
		assertFalse(a contains)
		assertFalse(a contains(nil))
		assertFalse(a contains(333))
		assertTrue(a contains("beta"))
	)
	testPush := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a push)
		a push(nil)
		a push(88)
		a push("blah", "fasel")
		assertEquals("fasel", a at(6))
		assertEquals(88, a at(4))
		assertNil(a at(3))
	)
	testPop := method(
		a := List clone append("a", "beta", 3)
		assertEquals(3, a pop)
		assertEquals(2, a size)
		assertEquals("beta", a pop)
		assertEquals(1, a size)
		assertEquals("a", a pop)
		assertEquals(0, a size)
		assertNil(a pop)
	)
	testEmpty := method(
		a := List clone append("a", "beta", 3)
		a empty
		assertEquals(0, a size)
	)
	testsize := method(
		a := List clone append("a", "beta", 3)
		assertEquals(0, List size)
		assertEquals(3, a size)
		a append("yo")
		assertEquals(4, a size)
	)
	testAtInsert := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a atInsert())
		assertRaisesException(a atInsert(nil))
		assertRaisesException(a atInsert(nil, "two"))
		assertRaisesException(a atInsert(-1, "two"))
		assertRaisesException(a atInsert(4, "two"))

		a atInsert(0, "zero")
		assertEquals(4, a size)
		assertEquals(0, a indexOf("zero"))

		a atInsert(1, "one")
		assertEquals(5, a size)
		assertEquals(1, a indexOf("one"))

		a atInsert(5, "append")
		assertEquals(6, a size)
		assertEquals(5, a indexOf("append"))
	)
	testRemoveAt := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a removeAt())
		assertRaisesException(a removeAt(nil))
		assertRaisesException(a removeAt(-1))
		assertRaisesException(a removeAt(3))

		a removeAt(0)
		assertEquals(2, a size)
		assertEquals("beta", a at(0))
		assertEquals(3, a at(1))

		a removeAt(1)
		assertEquals(1, a size)
		assertEquals("beta", a at(0))
	)
	testAt := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a at())
		assertRaisesException(a at(nil))
		assertNil(a at(-1))
		assertNil(a at(4))
		assertEquals("a", a at(0))
		assertEquals("beta", a at(1))
		assertEquals(3, a at(2))
	)
	testAtPut := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a atPut())
		assertRaisesException(a atPut(nil))
		assertRaisesException(a atPut(nil, "two"))
		assertRaisesException(a atPut(-1, "two"))
		assertRaisesException(a atPut(3, "two"))

		a atPut(0, "zero")
		assertEquals(3, a size)
		assertEquals(0, a indexOf("zero"))

		a atPut(2, "two")
		assertEquals(3, a size)
		assertEquals(2, a indexOf("two"))
	)
	testSwapIndices := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a swapIndices())
		assertRaisesException(a swapIndices(nil))
		assertRaisesException(a swapIndices(nil, nil))
		assertRaisesException(a swapIndices(1, nil))
		assertRaisesException(a swapIndices(nil, 1))
		assertRaisesException(a swapIndices(0, 3))
		assertRaisesException(a swapIndices(-1, 2))

		a swapIndices(0, 2)
		a swapIndices(1, 1)
		assertEquals("a", a at(2))
		assertEquals("beta", a at(1))
		assertEquals(3, a at(0))
	)
	testpreallocateToSize := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a preallocateToSize)
		assertRaisesException(a preallocateToSize(nil))
		a preallocateToSize(1)
		assertEquals(3, a size)
		a preallocateToSize(10)
		assertEquals(3, a size)
	)
	testFirst := method(
		a := List clone append("a", "beta", 3)
		assertNil(List first)
		assertEquals("a", a first)
	)
	testLast := method(
		a := List clone append("a", "beta", 3)
		assertNil(List last)
		assertEquals(3, a last)
	)
	_testPrint := method(
		// would need to be able to read stdout or install a printCallback from Io to test print()
		Nop
	)
	testSelect := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a select)
		selection := a select(index, value, index == 0 or value == 3)
		assertEquals(2, selection size)
		assertEquals("a", selection at(0))
		assertEquals(3, selection at(1))

		selection := a select(index, value, index == -1)
		assertEquals(0, selection size)
	)

	testSelect2 := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a select)
		selection := a select(value, value == "a" or value == 3)
		assertEquals(2, selection size)
		assertEquals("a", selection at(0))
		assertEquals(3, selection at(1))

		selection := a select(index, value, index == -1)
		assertEquals(0, selection size)
	)

	testSelectInPlace := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a selectInPlace)
		a selectInPlace(index, value, index == 0 or value == 3)
		assertEquals(2, a size)
		assertEquals("a", a at(0))
		assertEquals(3, a at(1))

		a selectInPlace(index, value, index == -1)
		assertEquals(0, a size)
	)

	testSelectInPlace2 := method(
		a := List clone append("a", "beta", 3)
		a selectInPlace(index, value, value == "a" or value == 3)
		assertEquals(2, a size)
		assertEquals("a", a at(0))
		assertEquals(3, a at(1))

		a selectInPlace(index, value, index == -1)
		assertEquals(0, a size)
	)
	testDetect := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a detect)
		assertRaisesException(a detect(nil))
		a detect(index, value, index == 0 or value == 3)
		assertEquals("a", a detect(index, value, index == 0 or value == 3))
		assertEquals(3, a detect(index, value, value type == "Number" and value > 1))
		assertNil(a detect(index, value, value > 3))
	)
	testDetect := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a detect)
		a detect(index, value, value = "a" or value == 3)
		assertEquals("a", a detect(index, value, index == 0 or value == 3))
		assertEquals(3, a detect(index, value, value type == "Number" and value > 1))
		assertNil(a detect(index, value, value type == "Number" and value > 3))
	)
	testSort := method(
		a := List clone append("a", "beta", "3")
		List clone sortInPlace
		a sortInPlace
		assertEquals("3", a at(0))
		assertEquals("a", a at(1))
		assertEquals("beta", a at(2))

		a append("0") sortInPlace
		assertEquals("0", a at(0))
		assertEquals("3", a at(1))
		assertEquals("a", a at(2))
		assertEquals("beta", a at(3))
	)

	testSortBy := method(
		a := List clone append("beta", "3", "alpha")
		assertRaisesException(a sortInPlaceBy)
		assertRaisesException(a sortInPlaceBy(nil))
		a sortInPlaceBy(block(v1, v2, v1 < v2))
		assertEquals("3", a at(0))
		assertEquals("alpha", a at(1))
		assertEquals("beta", a at(2))
	)

	testEmptyListSortBy := method(
		assertEquals(List clone sortInPlaceBy(block(x, y, x < y)), List clone)
	)

	testSortBySingleItem := method(
		assertEquals(list(3) sortInPlaceBy(block(x, y, x < y)), list(3))
	)

	testSortByTwoItems := method(
		assertEquals(list(3,1) sortInPlaceBy(block(x, y, x < y)), list(1,3))
	)

	testForeach := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a foreach)
		string := ""
		assertEquals("abeta3", a foreach(index, value, string := string .. value asString))

		string := ""
		assertEquals("abeta3", a foreach(value, string := string .. value asString))

		number := 0
		assertEquals(0+1+2, a foreach(index, value, number := number + index))
	)

	testReverse := method(
		a := List clone append("a", "beta", 3)
		reversedA := a reverse
		assertEquals(3, reversedA at(0))
		assertEquals("beta", reversedA at(1))
		assertEquals("a", reversedA at(2))
	)
	testReverseForeach := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a reverseForeach)
		assertRaisesException(a reverseForeach(nil))
		string := ""
		assertEquals("3betaa", a reverseForeach(index, value, string := string .. value asString))

		string := ""
		assertEquals("3betaa", a reverseForeach(value, string := string .. value asString))

		number := 0
		assertEquals(0+1+2, a reverseForeach(index, value, number := number + index))
	)

	testmap := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a map)
		a mapInPlace(index, value, value asString .. index asString)
		assertEquals(3, a size)
		assertEquals("a0", a at(0))
		assertEquals("beta1", a at(1))
		assertEquals("32", a at(2))
	)

	testmap2 := method(
		a := List clone append("a", "beta", 3)
		a mapInPlace(value, value asString)
		assertEquals(3, a size)
		assertEquals("a", a at(0))
		assertEquals("beta", a at(1))
		assertEquals("3", a at(2))
	)

	testMap := method(
		a := List clone append("a", "beta", 3)
		assertRaisesException(a map)
		assertRaisesException(a mapInPlace)
		a mapInPlace(index, value, value asString .. index asString)
		assertEquals(3, a size)
		assertEquals("a0", a at(0))
		assertEquals("beta1", a at(1))
		assertEquals("32", a at(2))
	)

	testMap2 := method(
		a := List clone append("a", "beta", 3)
		a mapInPlace(value, value asString)
		assertEquals(3, a size)
		assertEquals("a", a at(0))
		assertEquals("beta", a at(1))
		assertEquals("3", a at(2))
	)

	testAsMap := method(
		l := list(list("d", 4), list("c", 3), list("a", 1), list("b", 2))
		m := l asMap

		assertEquals(4, m size)

		l foreach(pair,
			key := pair at(0)
			value := pair at(1)

			assertTrue(m hasKey(key))
			assertEquals(value, m at(key))

			m removeAt(key)
		)

		assertEquals(0, m size)
	)

	testEncodedList := method(
		t := list(123)
		assertEquals(t, t asEncodedList asDecodedList)
		t := list(1, 2, 3)
		assertEquals(t, t asEncodedList asDecodedList)
		t := list(1, "test", nil, "foo", 3)
		assertEquals(t, t asEncodedList asDecodedList)
		t := list("foo")
		assertEquals(t, t asEncodedList asDecodedList)
	)
)
