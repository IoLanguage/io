ClosureTest := UnitTest clone do(
	// A method's locals object is recycled when the method returns, unless
	// something still refers to it. A block created in the method does: the
	// locals are its scope. These tests fail if such locals are recycled,
	// because the next method call wipes and reuses them.

	makeCounter := method(n, block(n = n + 1; n))
	makeAdder := method(x, block(y, x + y))
	churn := method(a, b, c, a)   // any method call takes locals from the pool

	testBlockKeepsItsScope := method(
		add5 := makeAdder(5)
		churn(1, 2, 3); churn(4, 5, 6)
		assertEquals(6, add5 call(1))
		assertEquals(list(6, 7, 8), list(1, 2, 3) map(v, add5 call(v)))
	)

	testClosuresAreIndependent := method(
		a := makeCounter(10)
		b := makeCounter(100)
		assertEquals(11, a call)
		assertEquals(101, b call)
		assertEquals(12, a call)
		churn(1, 2, 3)
		assertEquals(13, a call)
		assertEquals(102, b call)
	)

	testNestedClosures := method(
		outer := method(x, block(y, block(z, x + y + z)))
		f := outer(1) call(10)
		churn(7, 8, 9); churn(7, 8, 9)
		assertEquals(111, f call(100))
	)

	testStoredLocalsSurvive := method(
		capture := method(v, thisLocalContext)
		kept := capture(42)
		churn(1, 2, 3); churn(4, 5, 6)
		assertEquals(42, kept v)

		inList := method(v, l := list; l append(thisLocalContext); l)
		l := inList(7)
		churn(1, 2, 3)
		assertEquals(7, l first v)
	)

	testCoroutineKeepsSenderLocals := method(
		// coroFor runs its body in the sender's locals, after the sender
		// has returned
		start := method(v, coroFor(v * 2))
		c := start(21)
		churn(1, 2, 3); churn(4, 5, 6)
		c run
		assertEquals(42, c result)
	)

	testBlockCallEvaluatesAllArguments := method(
		// `aBlock call` evaluates every argument first, as any primitive does
		seen := list
		note := method(v, seen append(v); v)
		b := block(a, a)
		assertEquals(1, b call(note(1), note(2)))
		assertEquals(list(1, 2), seen)
	)

	testDeepRecursionThroughCall := method(
		// block call and forward use heap frames, not the C stack
		count := block(n, if(n == 0, "done", count call(n - 1)))
		assertEquals("done", count call(20000))

		o := Object clone
		o forward := method(n, if(n == 0, "bottom", self down(n - 1)))
		assertEquals("bottom", o down(20000))
	)
)
