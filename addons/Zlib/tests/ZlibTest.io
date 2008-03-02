Zlib

ZlibDecoderTest := UnitTest clone do(	
    testBasic := method(
        path := Path with(System launchPath, "test.gz")
        z := ZlibDecoder clone
        z setInputBuffer(File with(path) contents)
        z outputBuffer := Sequence clone
        z beginProcessing
        z process
        z endProcessing
        s := "TestSuite clone setPath(System launchPath) run"
        assertTrue(z outputBuffer containsSeq(s))
	)
) 
