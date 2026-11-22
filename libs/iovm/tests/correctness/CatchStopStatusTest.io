CatchStopStatusTest := UnitTest clone do(
	// A return, break or continue inside a catch handler acts on the method
	// or loop that called catch. The handler used to run inside the catch
	// method's own frame, so `catch(Exception, return x)` only left catch
	// and the enclosing method carried on (#37).

	MyError := Exception clone

	testReturnInHandler := method(
		f := method(
			try(Exception raise("x")) catch(Exception, return "okay")
			"not okay"
		)
		assertEquals(f, "okay")
	)

	testHandlerWithoutReturnFallsThrough := method(
		f := method(
			try(Exception raise("x")) catch(Exception, "handled")
			"fell through"
		)
		assertEquals(f, "fell through")
	)

	testNoExceptionSkipsHandler := method(
		f := method(try(1) catch(Exception, return "bad"); "fine")
		assertEquals(f, "fine")
	)

	testReturnInFirstMatchingHandler := method(
		f := method(
			try(MyError raise("x")) catch(MyError, return "mine") catch(Exception, return "general")
			"none"
		)
		assertEquals(f, "mine")
	)

	testReturnInLaterHandler := method(
		f := method(
			try(Exception raise("x")) catch(MyError, return "mine") catch(Exception, return "general")
			"none"
		)
		assertEquals(f, "general")
	)

	testBreakInHandler := method(
		n := 0
		10 repeat(n = n + 1; try(Exception raise("x")) catch(Exception, break))
		assertEquals(n, 1)
	)

	testContinueInHandler := method(
		seen := list()
		for(i, 1, 4,
			try(if(i == 2, Exception raise("x"))) catch(Exception, continue)
			seen append(i)
		)
		assertEquals(seen, list(1, 3, 4))
	)

	testCatchStillReturnsNilWhenHandled := method(
		assertNil(try(Exception raise("x")) catch(Exception, 42))
	)

	testUnmatchedExceptionPassesThrough := method(
		e := try(Exception raise("x")) catch(MyError, nil)
		assertEquals(e error, "x")
	)
)
