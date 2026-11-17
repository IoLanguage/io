CompileErrorTest := UnitTest clone do(
	// Unfinished or malformed source raises a "compile error" exception that
	// code can catch, and the VM carries on. These used to crash the VM: the
	// parser freed the lexer that its caller freed again, and raising the
	// error popped collector pauses that their owners popped again.

	compileError := method(src,
		e := try(doString(src))
		if(e, e error, nil)
	)

	testUnfinishedSourceRaises := method(
		list("x := (", "(", ")", "x := [", "foo(1, ", "\"abc", "\"\"\"abc", "a(b(c", "list(1,2") foreach(src,
			err := compileError(src)
			assertTrue(err != nil)
			assertTrue(err beginsWithSeq("compile error: "))
		)
	)

	testErrorSaysWhatAndWhere := method(
		assertEquals("compile error: \"unmatched ()s\" on line 1 character 8", compileError("list(1,2"))
		assertTrue(compileError("1\n(") containsSeq("on line 2 "))
	)

	testEveryEntryPointRaises := method(
		assertTrue(try("x := (" asMessage) error beginsWithSeq("compile error"))
		assertTrue(try(Compiler messageForString("(", "t")) error beginsWithSeq("compile error"))
		assertTrue(try(Lobby doString("1 + (")) error beginsWithSeq("compile error"))
		assertTrue(try(method(doString("x := ("); "not reached") call) error beginsWithSeq("compile error"))
	)

	testVmCarriesOnAfterManyErrors := method(
		500 repeat(compileError("x := ("); list(1, 2, 3) map(v, v asString))
		Collector collect
		assertEquals(14, doString("list(1, 2, 3) map(x, x * x) sum"))
	)

	testValidSourceStillCompiles := method(
		assertEquals(3, doString("1 + 2"))
		assertEquals(nil, compileError("x := (1 + 2) * 3"))
	)
)
