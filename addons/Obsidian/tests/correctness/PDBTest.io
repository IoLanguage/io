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
		richPpid := rich ppid
		
		richsName := "Rich Collins"
		rich name := richsName
		
		richsEmail := "rc@gmail.com"
		rich email := richsEmail
		
		pdb root setSlot(richsEmail, rich)
		pdb sync
		pdb close
		pdb open
		
		rich2 := pdb root at(richsEmail)
		
		assertEquals(rich2 name, rich name)
		assertEquals(rich2 email, rich email)
		pdb close
		//writeln("testObjectSync")
	)
)
