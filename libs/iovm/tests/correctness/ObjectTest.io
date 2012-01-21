
ObjectTest := UnitTest clone do(
	setUp := method(
		super(setUp)
	)

	tearDown := method(
		super(tearDown)
	)

	testProto := method(
		a := Object clone
		b := a clone
		assertSame(Object, a proto)
		assertSame(a, b proto)
	)

	testFor := method(
		assertEquals(5, for(i, 1, 10, if(i == 5, break(i))))
	)

	testHasLocalSlot := method(
		assertFalse(Object clone hasLocalSlot("clone"))
		assertTrue(Object hasLocalSlot("clone"))
	)

	testHasSlot := method(
		assertFalse(Object hasSlot("slotThatShouldNotExist"))
		assertTrue(Object clone hasSlot("clone"))
		assertTrue(Object hasSlot("clone"))
	)

	testActor := method(
		A := Object clone
		A s := Sequence clone
		A test := method(a, for(i, 1, 2, s appendSeq(a, i asString, "."); yield))
		A clone @@test("a");
		yield
		A clone @@test("b")
		yield; yield; yield; yield;
		assertEquals("a1.b1.a2.b2.", A s asString)
	)


	testFuture := method(
		obj := Object clone
		obj double := method(v, return v * 2)
		future := obj @double(2)
		assertEquals(future, 4)
		// known bug - assertEquals(4, future)
	)

	testQuestionMarkReturns := method(
		a := method(x, x return "first"; "second")
		b := method(x, x ?return "first"; "second")
		assertEquals(a, b)
	)

	testQuestionMarkContinues := method(
		a := method(
			r := list
			list(1,2,3,4,5) foreach(x,
				if(x > 3, continue)
				r append(x)
			)
			r
		)
		b := method(
			r := list
			list(1,2,3,4,5) foreach(x,
				if(x > 3, ?continue)
				r append(x)
			)
			r
		)
		assertEquals(a, b)
	)

	testForeachSlotStopStatus := method(
		f := method(
			r := list
			o := Object clone do(a := 1; b := 2; c := 3; d := 4; e := 5; f := 6)
			o foreachSlot(k, v,
				if(k == "a", continue)
				if(k == "e", break)
				r append(k)
			)
			o foreachSlot(k, v,
				if(k == "d", return r)
				r append(k)
			)
			r append("didn't return")
			r
		)

		assertEquals(f, list("b", "c", "d", "a", "b", "c"))
	)

	/*
	_willFree2 := method(
		Lobby willFreeWorked := false
		Object clone do(willFree := method(Lobby willFreeWorked := true))
		nil
	)

	testWillFree := method(
		_willFree2
		Collector collect
		assertEquals(Lobby willFreeWorked, true)
	)
	*/

	testLazySlot := method(
		o := Object clone
		o bobCalcCount := 0
		o lazySlot("bob", bobCalcCount = bobCalcCount + 1; 19)

		assertEquals(o bobCalcCount, 0)
		assertEquals(o bob, 19)
		assertEquals(o bobCalcCount, 1)
		assertEquals(o bob, 19)
		assertEquals(o bobCalcCount, 1)
		assertEquals(o bob, 19)
		assertEquals(o bobCalcCount, 1)
	)

	testRemoveAllProtosAsString := method(
		# shouldn't segfault
		assertRaisesException(Object clone removeAllProtos asString)
	)
)
