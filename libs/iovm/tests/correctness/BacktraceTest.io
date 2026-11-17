BacktraceTest := UnitTest clone do(
	// An exception records where it was raised (Exception stackTrace), taken
	// from the frame chain when it is raised. Backtraces used to be built from
	// Call objects on the retain stack, which the stackless evaluator no
	// longer leaves there, so nearly every error printed "nothing on stack".
	//
	// Each helper ends with a plain value so its call is not a tail call: a
	// tail call reuses its caller's frame, and the caller then has no entry.

	Thing := Object clone do(
		type := "Thing"
		outer := method(x, r := self inner(x); r)
		inner := method(x, r := x + nil; r)
		raiser := method(Exception raise("boom"); nil)
		missing := method(r := 1 noSuchMethod; r)
		looper := method(list(1, 2) foreach(v, if(v == 2, self inner(v))); nil)
	)

	// "Type name" of each trace line, without the file and line columns
	calls := method(e,
		e stackTraceLines map(line, line exSlice(0, 36) asMutable strip)
	)

	testErrorFromCShowsCallChain := method(
		e := try(Thing outer(1))
		assertEquals(list("Number +", "Thing inner", "Thing outer"), calls(e) slice(0, 3))
	)

	testRaiseShowsCallChain := method(
		e := try(Thing raiser)
		assertEquals(list("Exception raise", "Thing raiser"), calls(e) slice(0, 2))
	)

	testMissingMethodShowsFailedMessage := method(
		e := try(Thing missing)
		assertEquals(list("Number noSuchMethod", "Thing missing"), calls(e) slice(0, 2))
	)

	testCallsInsideLoopsAndBranches := method(
		e := try(Thing looper)
		assertEquals(list("Number +", "Thing inner", "Thing looper"), calls(e) slice(0, 3))
	)

	testLinesNameFileAndLine := method(
		e := try(Thing outer(1))
		// each line gives where that message was sent from
		assertTrue(e stackTraceLines at(0) containsSeq("BacktraceTest.io 13"))
		assertTrue(e stackTraceLines at(1) containsSeq("BacktraceTest.io 12"))
	)

	testPassKeepsTheOriginalTrace := method(
		e := try(Thing outer(1))
		e2 := try(e pass)
		assertTrue(e == e2)
		assertEquals(list("Number +", "Thing inner", "Thing outer"), calls(e2) slice(0, 3))
	)

	testBackTraceStringIncludesTheTrace := method(
		e := try(Thing outer(1))
		s := e coroutine backTraceString
		assertTrue(s containsSeq("Thing inner"))
		assertFalse(s containsSeq("nothing on stack"))
	)

	testCoroutineErrorHasItsOwnTrace := method(
		o := Object clone
		o caught := nil
		o handleActorException := method(e, self caught = e)
		o thing := Thing
		o work := method(r := thing outer(1); r)
		o @@work
		5 repeat(yield)
		assertTrue(o caught != nil)
		assertEquals(list("Number +", "Thing inner", "Thing outer"), calls(o caught) slice(0, 3))
	)

	testNeverRaisedHasNoTrace := method(
		assertEquals(list(), Exception clone stackTraceLines)
	)

	testDeepRecursionIsCapped := method(
		down := method(n, r := if(n == 0, 1 + nil, down(n - 1)); r)
		e := try(down(500))
		assertTrue(e stackTrace size <= 128)
		assertEquals("Number +", calls(e) first)
	)
)
