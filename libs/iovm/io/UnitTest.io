//metadoc UnitTest category Test
/*metadoc UnitTest description
An object for organizing and running unit tests validated by assertions.
*/

//doc UnitTest setUp Method called prior to each test.
//doc UnitTest tearDown Method called after each test.
//doc UnitTest testCount Returns the number of tests defined in this object.
//doc UnitTest run Runs all tests.
//doc UnitTest fail Call to trigger a test failure.
UnitTest := Object clone do(
    width := 60 # Line width.

    init := method(self exceptions := List clone)

    setUp := method(nil)
    tearDown := method(nil)

    testSlotNames := method(
        names := self slotNames select(beginsWithSeq("test"))
        if(names isEmpty,
            names
        ,
            names sortByKey(name, self getSlot(name) message lineNumber)
        )
    )

    testCount := method(testSlotNames size)

    run := method(
        start := Date clone now
        testSlotNames foreach(idx, name,
            self setUp
            # Break the line, when the number of dots exceed the
            # predefined line width.
            if(idx > 0 and idx % width == 0, "\n" print)

            exc := try(stopStatus(self doString(name)))
            if(exc,
                exceptions append(list(name, exc))
                "E" print
            ,
                "." print
            )
            self tearDown
        )
        stop := Date clone now

        "\n" print

        # Printing exceptions in the order they occured.
        exceptions foreach(excdata,
            # The header is contains both test object type and slot,
            # if the former differs from UnitTest.
            caseName := if(self type != "UnitTest", self type .. " ", "")
            testName := excdata at(0)
            exc := excdata at(1)

            ("=" repeated(width) .. "\nFAIL: " .. caseName .. testName .. \
             "\n" .. "-" repeated(width)) println
            exc showStack
        )

        # Printing out test summary; this code, and some of the above
        # code need to be moved to the TestRunner class, responsible
        # for running a list of tests and displaying the results.
        "-" repeated(width) println
        ("Ran " .. testCount .. " test" .. if(testCount != 1, "s", "") .. \
         " in " .. stop secondsSince(start) .. "\n") println

        result := if(exceptions isNotEmpty,
            "FAILED (failures #{exceptions size})" interpolate
        ,
            "OK")

        # Note: a dirty hack, the same as in Coroutine showStack though :)
        # However, if the method body is empty, the label will be [unlabeled],
        # so this is not a universal solution.
        filename := if (testSlotNames isNotEmpty,
            self getSlot(testSlotNames first) message label fileName
        ,
            "")
        (result .. filename alignRight(width - result size) .. "\n") println

        # Cleaning up.
        self ?cleanUp
    )

    fail := method(Exception raise("fail"))

//doc UnitTest assertEquals(a, b) Fail the running test if a != b.
//doc UnitTest assertNotEquals(a, b) Fail the running test if a == b.
//doc UnitTest assertNotSame(a, b) Fail the running test if a != b.
//doc UnitTest assertNil(a) Fail the running test if a != nil.
//doc UnitTest assertNotNil(a) Fail the running test if a == nil.
//doc UnitTest assertTrue(a) Fail the running test if a != true.
//doc UnitTest assertFalse(a) Fail the running test if a != false.
//doc UnitTest assertRaisesException(code) Fail the running test if the code does not raise an Exception.
/*doc UnitTest assertEqualsWithinDelta(expected, actual, delta)
Fail the running test if the expected value is not within delta of the actual value.
*/

    assertEquals := method(a, b, m,
        //writeln("assertEquals1 call message = ", call message type)
        mm := call message
        if(m == nil, m = mm)
        d := m argAt(0) code .. " != " .. call argAt(1) code
        if(a != b, Exception raise("[" .. d .. "] [" .. a asSimpleString .. " != " .. b asSimpleString .. "]"))
        //writeln("assertEquals2")
    )

    assertNotEquals := method(a, b, if(a == b, Exception raise(a asSimpleString .. " == " .. b asSimpleString)))

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

//metadoc TestSuite category Testing
/*metadoc TestSuite description
An object to collect and run multiple UnitTests defined in *Test.io files within the System launchPath directory.
*/
TestSuite := Object clone do(
    path ::= "."

//doc TestSuite name Return the name of the TestSuite.
    name := method(path asMutable pathComponent lastPathComponent)
//doc TestSuite with(aPath) Returns a new instance with the provided path.
    with := method(path, self clone setPath(path))

    testFiles := method(
        Directory with(System launchPath) files select(name endsWithSeq("Test.io"))
    )

/*doc TestSuite run
Collects and all files named *Test.io within the System launchPath directory,
runs their tests and prints collated results.
*/
    run := method(
        exceptions := List clone

        testFiles foreach(file,
            test := Lobby doString(file contents, file path)
            test run
            exceptions appendSeq(test exceptions)
        )

        return(exceptions size)
    )
)