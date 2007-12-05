UnitTest := Object clone do(
	verbose := method(s,
		nil
		//writeln(s)
	)
	
	docCategory("Testing")
	setUp := method(nil)
	tearDown := method(nil)
	init := method(self exceptions := List clone)
	testSlotNames := method(self slotNames select(n, n beginsWithSeq("test")) sortInPlace)
	
	testCount := method(testSlotNames size)
	
	run := method(		
		testSlotNames foreach(n, 
			self setUp
			verbose("    " .. n)
			e := try(stopStatus(self doString(n)))
			if(e, 
				write("    ", n, " - failed")
				exceptions append(e)
				write("\n")
				writeln(e showStack)
				//System exit
			)
			//Scheduler currentCoroutine yield
			self tearDown
			//Collector collect
		)
		self ?cleanUp
	)
    
    fail := method(Exception raise("fail"))
    
	assertEquals := method(a, b, m,
		//writeln("assertEquals1 call message = ", call message type)
		mm := call message
		if(m == nil, m = mm)
		d := m argAt(0) code .. " != " .. call argAt(1) code
		if(a != b, Exception raise("[" .. d .. "] [" .. a .. " != " .. b .. "]"))
		//writeln("assertEquals2")
	)

	assertNotEquals := method(a, b, if(a == b, Exception raise(a .. " == " .. b)))

    assertSame    := method(a, b, assertEquals(a uniqueId, b uniqueId, call message))
    assertNotSame := method(a, b, assertNotEquals(a uniqueId, b uniqueId, call message))
    assertNil     := method(a, assertEquals(a, nil, call message))
    assertNotNil  := method(a, assertNotEquals(a, nil, call message))
    assertTrue    := method(a, assertEquals(a, true, call message))
    assertFalse   := method(a, assertEquals(a, false, call message))

	assertRaisesException := method(
		e := try(
			stopStatus(call evalArgAt(0))
			writeln("Should have raised Exception")
		)
		e ifNil(Exception raise("Should have raised Exception"))
	)
    
	knownBug := method(
		//writeln("  [known bug: ", call argAt(0) code, "]")
	)
  
	assertEqualsWithinDelta := method(expected, actual, delta,
		if(((expected - actual) abs > delta),
			Exception raise("expected " .. expected .. " but was " .. actual .. " (allowed delta: " .. delta .. ")")
		)
	)
)

TestSuite := Object clone do(
	verbose := method(s,
		nil
		//writeln(s)
	)
	
	docCategory("Testing")
	
	name := method(
		path asMutable pathComponent lastPathComponent
	)
	
	newSlot("path", ".")
	
	run := method(
		verbose("\n" .. name)
		unitTestFiles := Directory with(launchPath) files select(f, f name endsWithSeq("Test.io"))
		exceptions := List clone
		testCount := 0
				
		unitTestFiles foreach(f, 
			1 repeat(
				verbose("  " .. f name fileName)
				test := Lobby doString(f contents, f path)
				//Collector collect
				test run
			)
			testCount = testCount + test testCount
			exceptions appendSeq(test exceptions)
		)
		
		verbose("  ---------------")
		//Collector collect
		//if(testCount == 0, write("(no tests)"); File standardOutput flush; return(0))
		if(exceptions size > 0) then(
			//writeln("	FAILED ", testCount, " tests, ", exceptions size, " failures\n")
			return(exceptions size)
		) else(
			//writeln("	PASSED ", testCount, " tests\n")
			return(0)
		)
	)
)
