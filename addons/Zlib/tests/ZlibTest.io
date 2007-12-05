Zlib

ZlibDecoderTest := UnitTest clone do(	
    testBasic := method(
        path := Path with(launchPath, "test.gz")
        z := ZlibDecoder clone
        z setInputBuffer(File with(path) contents)
        z outputBuffer := Sequence clone
        z beginProcessing
        z process
        z endProcessing
        //z outputBuffer println
        s := "TestSuite clone setPath(launchPath) run"
        assertTrue(z outputBuffer containsSeq(s))
	)
) 
