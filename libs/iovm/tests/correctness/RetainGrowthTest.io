RetainGrowthTest := UnitTest clone do(
	// Code that keeps running (a top-level loop, a coroutine or actor that
	// never returns) must not keep everything it ever computed alive.
	// Values created in a loop body used to stay on the retain stack until
	// the enclosing block returned; for and foreach counters popped other
	// entries off it when the counter was a cached small integer; and every
	// coroutine switch left an orphaned retain pool behind. Each case below
	// grew by about one object per iteration or switch.

	liveObjects := method(Collector collect; Collector allObjects size)

	// Runs aBlock and returns how many more objects are alive afterwards.
	growthOf := method(aBlock,
		before := liveObjects
		aBlock call
		liveObjects - before
	)

	// A long loop measured from inside the same block that runs it, as a
	// top-level script or a never-ending coroutine would be.
	testLoopsDoNotAccumulate := method(
		n := 20000
		assertTrue(growthOf(block(for(i, 1, n, i + 1))) < 100)
		assertTrue(growthOf(block(j := 0; while(j < n, j = j + 1))) < 100)
		assertTrue(growthOf(block(k := 0; loop(k = k + 1; if(k > n, break)))) < 100)
		l := list(); n repeat(l append(1))
		assertTrue(growthOf(block(l foreach(i, v, v + i))) < 100)
	)

	testLoopsMeasuredFromInside := method(
		b := liveObjects
		for(i, 1, 20000, i + 1)
		j := 0; while(j < 20000, j = j + 1; j asString)
		assertTrue(liveObjects - b < 100)
	)

	testCachedIntegerCountersLeaveOuterRetainsAlone := method(
		// A body of just `i` creates nothing; the counter values are cached
		// integers. This used to pop 20000 unrelated retain stack entries.
		for(i, 1, 20000, i)
		list(1, 2, 3) foreach(i, v, i)
		b := liveObjects
		j := 0; while(j < 20000, j = j + 1)
		assertTrue(liveObjects - b < 100)
	)

	testCoroutineSwitchingDoesNotAccumulate := method(
		done := false
		coro := coroDo(while(done not, yield))
		yield
		b := liveObjects
		i := 0; while(i < 10000, i = i + 1; yield)
		assertTrue(liveObjects - b < 100)
		done = true; 3 repeat(yield)
	)

	testActorDoesNotAccumulate := method(
		a := Object clone do(total := 0; add := method(x, total = total + x))
		a @@add(1); 3 repeat(yield)
		b := liveObjects
		5000 repeat(a @@add(1); yield)
		while(a actorQueue ?size > 0, yield)
		assertEquals(5001, a total)
		assertTrue(liveObjects - b < 100)
	)

	testLoopResultsAreStillRight := method(
		assertEquals(20001, for(i, 1, 20000, i + 1))
		assertEquals(list(2, 4, 6), list(1, 2, 3) map(v, v * 2))
		r := list(); for(i, 1, 3, r append(i asString)); assertEquals(list("1", "2", "3"), r)
		assertEquals(9, j := 0; while(j < 9, j = j + 1))
		assertEquals(5, list(1, 2, 3, 4, 5) foreach(v, v))
		assertEquals("done", for(i, 1, 10, if(i == 3, break("done"))))
	)
)
