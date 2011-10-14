// current state that this test checks:
// 1. io input files are utf-8
// 2. strings parsed from io input files are ascii/utf16/utf32
// 3. io standard printing emits utf-8
// 4. file read/write does not change string encodings
// 5. argument treating is the same as for input code

UnicodeTest := UnitTest clone do(
	io := Path with("_build", "binaries", "io_static ") # Yep, ugly, but at least it works :)

	wdPath := Path with(method(call message label pathComponent) call, "UnicodeTest-helper")
	tempPath := Path with(wdPath, "UnicodeTest.tmp")
	textPath := Path with(wdPath, "UnicodeTest.txt")

	fileString := File with(textPath) contents
	triQuoteString := \
"""Hello, world!
Здравствуй, мир!
この世界お。今日は！
"""
	monoQuoteString := "Hello, world!\nЗдравствуй, мир!\n"
	monoQuoteString = monoQuoteString .. "この世界お。今日は！\n"

	plat := System platform
	isOnWindows := plat beginsWithSeq("Windows") or plat beginsWithSeq("mingw")
	if(isOnWindows,
		diffCmd := "diff -q --strip-trailing-cr ",
		diffCmd := "diff -q "
	)

	tempWrite := method(s,
		File with(tempPath) openForUpdating truncateToSize(0) write(s) close
	)

	tempSystem := method(s,
		code := (io .. s) asMutable replaceSeq("$0", Path with(wdPath, "printer.io"))
		if(isOnWindows, code := code replaceSeq("\n", "\\n"))
		System system(code .. " > " .. tempPath)
	)

	assertDiff := method(
		outcome := System system(diffCmd .. tempPath .. " " .. textPath)
		File with(tempPath) remove
		assertTrue(outcome == 0)
	)

	knownBugDiff := method(
		File with(tempPath) remove
		knownBug(System system(diffCmd .. tempPath .. " " .. textPath) == 0)
	)

	testCompares := method(
		# do not use assertEquals to avoid damaging terminal
		assertTrue(triQuoteString asUTF8 == fileString asUTF8)
		assertTrue(triQuoteString asUTF8 == monoQuoteString asUTF8)
		assertTrue(triQuoteString asUTF8 == fileString asUTF8)
	)

	testFileFile := method(
		tempWrite(fileString)
		assertDiff
	)

	testFileTriQuote := method(
		tempWrite(triQuoteString asUTF8)
		assertDiff
	)

	testFileMonoQuote := method(
		tempWrite(monoQuoteString asUTF8)
		assertDiff
	)

	testPrintFile := method(
		tempSystem("$0 --print fileString")
		assertDiff
	)

	testPrintTriQuote := method(
		tempSystem("$0 --print triQuoteString")
		assertDiff
	)

	testPrintMonoQuote := method(
		tempSystem("$0 --print monoQuoteString")
		assertDiff
	)

	testArgsMonoQuote := method(
		tempSystem("$0 --arg \"" .. monoQuoteString asUTF8 .. "\"")
		assertDiff
	)

	testArgsTriQuote := method(
		tempSystem("$0 --arg \"" .. triQuoteString asUTF8 .. "\"")
		assertDiff
	)

	testArgsFile := method(
		tempSystem("$0 --arg \"" .. fileString asUTF8 .. "\"")
		assertDiff
	)

	testArgsEval := method(
		// this removeLast thing is here to remove last "\n",
		// which io interpreter adds after executing the code
		string := monoQuoteString asMutable asUTF8 removeLast escape
		tempSystem("-e \"\\\"#{string}\\\" println\"" interpolate)
		assertDiff
	)

)
