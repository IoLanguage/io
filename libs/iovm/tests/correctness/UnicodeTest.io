// Tests for Unicode string handling.
// Subprocess-dependent tests (diff, System system) removed: not available on WASM.

UnicodeTest := UnitTest clone do(
	wdPath := Path with(method(call message label pathComponent) call, "UnicodeTest-helper")
	textPath := Path with(wdPath, "UnicodeTest.txt")

	fileString := File with(textPath) contents
	triQuoteString := \
"""Hello, world!
Здравствуй, мир!
この世界お。今日は！
"""
	monoQuoteString := "Hello, world!\nЗдравствуй, мир!\n"
	monoQuoteString = monoQuoteString .. "この世界お。今日は！\n"

	testCompares := method(
		# do not use assertEquals to avoid damaging terminal
		assertTrue(triQuoteString asUTF8 == fileString asUTF8)
		assertTrue(triQuoteString asUTF8 == monoQuoteString asUTF8)
		assertTrue(triQuoteString asUTF8 == fileString asUTF8)
	)

)
