NonAsciiSourceTest := UnitTest clone do(
	// A Sequence holding a non-ASCII character is stored with two or four
	// bytes per character. Code that read such a Sequence's bytes as a C
	// string stopped at the first zero byte, so compiling source text from
	// one failed or silently used only its first character.

	testDoString := method(
		assertEquals(doString("\"é\" size"), 1)
		assertEquals(doString("y := \"日本\"; y size"), 2)
		assertEquals(doString("x := 1 // é\nx + 1"), 2)
	)

	testDoStringResultKeepsText := method(
		assertEquals(doString("\"日本\" .. \"語\""), "日本語")
	)

	testAsMessageAndFromString := method(
		s := "y := \"日本\"; y size"
		assertEquals(s asMessage asString, "setSlot(\"y\", \"日本\") ;\ny size")
		assertEquals(Message fromString(s) asString, "setSlot(\"y\", \"日本\") ;\ny size")
		assertEquals(doMessage(s asMessage), 2)
	)

	testCompilerMessageForString := method(
		assertEquals(doMessage(Compiler messageForString("\"é\" size")), 1)
	)

	testMessagePrintsNonAsciiLiteral := method(
		assertEquals(message("é" size) asString, "\"é\" size")
		assertEquals(message(y := "日本") asString, "setSlot(\"y\", \"日本\")")
	)

	testInterpolateInNonAsciiString := method(
		assertEquals("日本#{1 + 1}語" interpolate, "日本2語")
		assertEquals("a#{1 + 1}語" interpolate, "a2語")
		assertEquals("#{\"ñ\" size}" interpolate, "1")
		s := "é#{2 * 3}" asMutable
		s interpolateInPlace
		assertEquals(s, "é6")
	)

	testAsciiStillWorks := method(
		assertEquals(doString("1 + 1"), 2)
		assertEquals("a #{1 + 1} b" interpolate, "a 2 b")
	)
)
