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
		pdb reopen
		
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
		pdb reopen


		pdb root removeSlot(rich email)
		pdb sync
		assertEquals(pdb root at(rich email), nil)
		pdb reopen
			
		rich2 := pdb root at(rich email)
		
		assertEquals(rich2, nil)
		
		pdb collectGarbage
		rich = nil
		Collector collect
		assertEquals(pdb db size, 1)
		pdb close
		pdb delete
	)

  testListSync := method(
    pdb open
    aList := list("1", "2", "3") 
    pdb root setSlot("aList", aList)
    pdb sync
    pdb reopen

    assertEquals(aList, pdb root at("aList"))
    assertEquals(aList, pdb objectAtPpid(aList ppid))
    assertEquals(aList ppid, pdb root at("aList") ppid)
  )

  testSequenceSync := method(
    pdb open
    aSeq := "aoeu"
    pdb root setSlot("aSeq", aSeq)
    pdb sync
    pdb reopen

    assertEquals(aSeq, pdb root at("aSeq"))
    assertEquals(aSeq, pdb objectAtPpid(aSeq ppid))
    assertEquals(aSeq ppid, pdb root at("aSeq") ppid)
  )

  testNumberSync := method(
    pdb open
    aNum := 10
    pdb root setSlot("aNum", aNum)
    pdb sync
    pdb reopen

    assertEquals(aNum, pdb root at("aNum"))
    assertEquals(aNum, pdb objectAtPpid(aNum ppid))
    assertEquals(aNum ppid, pdb root at("aNum") ppid)
  )
)
