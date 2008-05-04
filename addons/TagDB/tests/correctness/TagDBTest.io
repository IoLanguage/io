TagDB

TagDBTest := UnitTest clone do(
	
	testBasic := method(
		tdb := TagDB clone
		
		tdb setPath("test")
		tdb delete
		tdb open
		
        assertEquals(tdb size, 0)
        
		tdb atKeyPutTags("f430 for sale", list("red", "ferrari"))
        assertEquals(tdb tagsAtKey("f430 for sale"), list("red", "ferrari"))
        tdb atKeyPutTags("lotus esprit", list("lotus", "esprit"))
        assertEquals(tdb tagsAtKey("lotus esprit"), list("lotus", "esprit"))
        
        assertEquals(tdb size, 2)
        
        keys := tdb keysForTags(list("lotus"))
        assertEquals(keys size, 1)
        assertEquals(tdb symbolForId(keys at(0)), "lotus esprit")

        tdb removeKey("lotus esprit")
        assertEquals(tdb size, 1)
        keys := tdb keysForTags(list("lotus"))
        assertEquals(keys size, 0)
        
        tags := tdb tagsAtKey("lotus esprit")
        assertEquals(tags, nil)
       
		writeln("ok")
		tdb close
		tdb delete
	)
)

