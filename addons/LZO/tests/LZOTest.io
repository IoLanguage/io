
LZO

LZOTest := UnitTest clone do(	
    testBasic := method(
        s := "abcabc abcabc abcabc abcabc abcabc abcabc"
        
        b := s asBuffer 
        uncompressedSize := b size
        
        b zCompress
        compressedSize := b size

        assertTrue(compressedSize < uncompressedSize)
        
        b zUncompress
        
        assertEquals(b asString, s)
	)
) 
