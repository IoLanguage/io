ListSchwartzianTest := UnitTest clone do(

	testSimpleOrdering := method(
		assertEquals(list(1,2,3,4) sortByKey(x, x), list(1,2,3,4))
		assertEquals(list(4,3,2,1) sortByKey(x, x), list(1,2,3,4))
	)

	testEmptyList := method(
		assertEquals(list sortByKey(x, x), list)
	)

	testSingleItemList := method(
		assertEquals(list(3) sortByKey(x, x), list(3))
	)

	testTwoItemList := method(
		assertEquals(list(3,1) sortByKey(x, x), list(1,3))
	)

	testThreeItemList := method(
		assertEquals(list(3,1,2) sortByKey(x, x), list(1,2,3))
	)

	testFourItemList := method(
		assertEquals(list(3,1,2,4) sortByKey(x, x), list(1,2,3,4))
	)

	testKeys := method(
		assertEquals("abc de fghi" split sortByKey(size), list("de", "abc", "fghi"))
	)

	if(isLaunchScript,
		verbose := getSlot("writeln")
		run
	)
)
