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

	/*
	testSyncWithPersistentSlots := method(
		rich := Object withPpid pSlots(name email)
		richPpid := rich ppid
		
		richsName := "Rich Collins"
		rich name := richsName
		
		richsEmail := "richcollins@gmail.com"
		rich email := richsEmail
		
		pdb sync
		
		namePpid := rich name ppid
		emailPpid := rich email ppid
		
		unpersistedRich := pdb objectAtPpid(richPpid)
		unpersistedName := unpersistedRich name
		assertEquals("Rich Collins", unpersistedName)
		
		unpersistedEmail := unpersistedRich email
		assertEquals("richcollins@gmail.com", unpersistedEmail)
	)
	
	testSyncWithPersistentNilSlot := method(
		rich := Object withPpid pSlots(name)
		richPpid := rich ppid
		
		richsName := "Rich Collins"
		
		pdb sync
		
		unpersistedRich := pdb objectAtPpid(richPpid)
		assertEquals(nil, unpersistedRich name)
	)
	
	testSyncWithExistingObject := method(
		clock := Object withPpid pSlots(time)
		clockPpid := clock ppid
		
		clock time := Date clone now
		
		pdb sync
		
		unpersistedClock := pdb objectAtPpid(clockPpid)
		unpersistedTime := unpersistedClock time
		unpersistedTime now
		
		pdb sync
		pdb emptyPpidMap
		
		unpersistedClock := pdb objectAtPpid(clockPpid)
		assertEquals(unpersistedTime, unpersistedClock time)
	)
)
	*/
)
