PDB

PDBTest := UnitTest clone do(
	setUp := method(
		Object pdb := PDB clone setPath("pobject_test.tc") open
		Collector cleanAllObjects
	)
	
	cleanUp := method(
		File with("pobject_test.tc") remove
	)
	
	testSimpleSync := method(
		rich := "Rich" withPpid
		ppid := rich ppid
		rich foo := 1
		pdb sync
		
		unpersistedPObj := pdb objectAtPpid(ppid)
		assertEquals("Rich", unpersistedPObj)
		assertEquals(rich ppid, unpersistedPObj ppid)
		assertEquals(rich type, unpersistedPObj type)
	)
	
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
)