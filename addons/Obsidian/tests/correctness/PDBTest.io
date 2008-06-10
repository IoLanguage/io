PDB

PDBTest := UnitTest clone do(
	setUp := method(
		self pdb := PDB setPath("test.tc") 
	)
	
	cleanUp := method(
		pdb delete
	)

	testSimpleSync := method(
		// store a value in the root PMap
		pdb open
		pdb root a := "a"
		pdb root b := 1
		pdb root c := nil
		pdb sync
		pdb close
		
		// see if it's still there after a reopen
		pdb open
		assertEquals(pdb root a, "a")
		assertEquals(pdb root b, 1)
		assertEquals(pdb root c, nil)
		pdb close
	)

	testObjectSync := method(
		pdb open
		rich := Object clone pSlots(name, email)		
		rich setName("Rich Collins")
		rich setEmail("rc@gmail.com")
		
		pdb root setSlot(rich email, rich)
		pdb repoen
		
		rich2 := pdb root at(rich email)
		assertEquals(rich2 name, rich name)
		assertEquals(rich2 email, rich email)
		pdb close
		pdb delete
		//writeln("testObjectSync")
	)
	
	testRemove := method(
		pdb open
		rich := Object clone pSlots(name, email)	
		rich setName("Rich Collins")
		rich setEmail("rc@gmail.com")
		pdb root setSlot(rich email, rich)
		pdb repoen


		pdb root removeSlot(rich email)
		pdb sync
		assertEquals(pdb root at(rich email), nil)
		pdb repoen
			
		rich2 := pdb root at(rich email)
		
		assertEquals(rich2, nil)
		
		pdb collectGarbage
		rich = nil
		Collector collect
		assertEquals(pdb db size, 1)
		pdb close
		pdb delete
	)
)
