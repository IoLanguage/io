SandboxTest := UnitTest clone do(
	// Sandbox setTimeLimit and setMessageCount used to store a number that
	// nothing read, so code in a Sandbox could run forever.

	testRunsCode := method(
		s := Sandbox clone
		assertEquals(s doSandboxString("1 + 2"), 3)
		s doSandboxString("x := 5")
		assertEquals(s doSandboxString("x * 2"), 10)
		assertEquals(s doSandboxString("\"a\" .. \"b\""), "ab")
	)

	testTimeLimit := method(
		s := Sandbox clone setTimeLimit(0.05)
		start := Date clone now
		e := try(s doSandboxString("loop(nil)"))
		assertEquals(e error, "Sandbox: time limit exceeded")
		assertTrue(Date clone now secondsSince(start) < 2)
	)

	testTimeLimitCantBeCaughtInside := method(
		s := Sandbox clone setTimeLimit(0.05)
		e := try(s doSandboxString("x := 0; loop(try(loop(x = x + 1)))"))
		assertEquals(e error, "Sandbox: time limit exceeded")
		e := try(s doSandboxString("e := try(loop(nil)); loop(nil)"))
		assertEquals(e error, "Sandbox: time limit exceeded")
	)

	testSandboxUsableAfterLimit := method(
		s := Sandbox clone setTimeLimit(0.05)
		try(s doSandboxString("x := 0; loop(x = x + 1)"))
		assertTrue(s doSandboxString("x") > 100)
		assertEquals(s doSandboxString("x = 7; x + 1"), 8)
	)

	testTimeLimitAppliesToEachCall := method(
		s := Sandbox clone setTimeLimit(0.05)
		try(s doSandboxString("loop(nil)"))
		assertEquals(s doSandboxString("n := 0; for(i, 1, 1000, n = n + i); n"), 500500)
	)

	testMessageCountLimit := method(
		s := Sandbox clone setMessageCount(500)
		e := try(s doSandboxString("x := 0; for(i, 1, 1000000, x = i)"))
		assertEquals(e error, "Sandbox: message count limit exceeded")
		x := s doSandboxString("x")
		assertTrue(x > 0 and x < 500)
		// Short code fits within the limit, on every call
		assertEquals(s doSandboxString("for(i, 1, 20, x = i); x"), 20)
		assertEquals(s doSandboxString("for(i, 1, 20, x = i); x"), 20)
	)

	testMessageCountLimitStopsEmptyLoops := method(
		s := Sandbox clone setMessageCount(2000)
		e := try(s doSandboxString("for(i, 1, 1000000000, nil)"))
		assertEquals(e error, "Sandbox: message count limit exceeded")
		e := try(s doSandboxString("loop(nil)"))
		assertEquals(e error, "Sandbox: message count limit exceeded")
		e := try(s doSandboxString("f := method(f); f"))
		assertEquals(e error, "Sandbox: message count limit exceeded")
	)

	testNoLimitByDefault := method(
		s := Sandbox clone
		assertEquals(s doSandboxString("n := 0; for(i, 1, 100000, n = n + i); n"), 5000050000)
		s setTimeLimit(0.05) setMessageCount(100)
		s setTimeLimit(0) setMessageCount(0)
		assertEquals(s doSandboxString("n := 0; for(i, 1, 100000, n = n + i); n"), 5000050000)
	)

	testLimitsDontReachTheOuterVM := method(
		s := Sandbox clone setMessageCount(100)
		try(s doSandboxString("loop(nil)"))
		n := 0
		for(i, 1, 50000, n = n + 1)
		assertEquals(n, 50000)
	)
)
