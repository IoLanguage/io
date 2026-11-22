TryStopStatusTest := UnitTest clone do(
	// The code given to try runs in its own coroutine, so a return, break
	// or continue in it used to end only the try: `method(try(return 5); 6)`
	// returned 6. try now passes the stop on to its caller.

	testReturnInBody := method(
		f := method(try(return 5); 6)
		assertEquals(f, 5)
	)

	testBreakInBody := method(
		r := list()
		for(i, 1, 3, try(if(i == 2, break)); r append(i))
		assertEquals(r, list(1))
	)

	testContinueInBody := method(
		r := list()
		for(i, 1, 3, try(if(i == 2, continue)); r append(i))
		assertEquals(r, list(1, 3))
	)

	testReturnFromLoopInBody := method(
		f := method(try(list(1, 2, 3) foreach(v, if(v == 2, return v * 10))); "end")
		assertEquals(f, 20)
	)

	testReturnThroughNestedTry := method(
		f := method(try(try(return "inner")); "outer")
		assertEquals(f, "inner")
	)

	testReturnAfterYield := method(
		f := method(try(yield; return "after yield"); "end")
		assertEquals(f, "after yield")
	)

	testReturnInBodySkipsCatch := method(
		f := method(try(return "body") catch(Exception, return "handler"); "end")
		assertEquals(f, "body")
	)

	testNormalBodyStillReturnsNil := method(
		f := method(x := try(42); x)
		assertNil(f)
		g := method(try(1 + 1); "normal")
		assertEquals(g, "normal")
	)

	testExceptionStillReturned := method(
		f := method(e := try(Exception raise("x")); e error)
		assertEquals(f, "x")
	)

	testFinalStopStatus := method(
		c := Coroutine clone
		c setRunTarget(Lobby) setRunLocals(Lobby) setRunMessage(message(1 + 1))
		c run
		assertTrue(c finalStopStatus isReturn not)
		assertTrue(c finalStopStatus isBreak not)
	)
)
