//metadoc UnitTest category Testing
/*metadoc UnitTest description
An object for organizing and running unit tests validated by assertions.
*/

//metadoc TestRunner category Testing
/*metadoc TestRunner description
Core testing object responsible for running tests, collected by UnitTests and TestSuites.
*/
TestRunner := Object clone do(
    width ::= 70 # Line width.

    init := method(
        self cases := nil
        self exceptions := List clone
        self runtime := 0
    )
//doc TestRunner testCount Returns the number of tests to be ran.
    testCount := method(
        self cases values prepend(0) reduce(
            count, testSlotNames, count + testSlotNames size
        )
    )

//doc TestRunner name Returns the name of the TestRunner.
    name := method(
        # If we are running a single test, the the test's name
        # is taken as TestRunner's name, else processed file
        # name is returned.
        if(self cases size > 1,
            System launchScript fileName
        ,
            if(self cases size > 0, self cases keys first, "")
        )
    )

    linebreak := method(
        if(self ?dots,
            self dots = self dots + 1
         ,
            self dots := 1)
        # Break the line, when the number of dots exceed the
        # predefined line width.
        if(self dots % width == 0, "\n" print)

    )
    success := method(
        "." print
        linebreak
    )
    error := method(name, exc,
        # We need to store both the name of the failed slot and
        # the exception occurred, so that we can output them together
        # later on.
        self exceptions append(list(name, exc))
        "E" print
        linebreak
    )

/*doc TestRunner run(testMap)
Runs all tests from a given Map object, where keys are names of the UnitTests
to run and values - lists of test slots theese UnitTests provide.

Returns a `list` which is empty if the tests passed or contains exceptions on 
failures.*/
    run := method(testMap,
        self cases := testMap # Storing a reference to the test map.
        self runtime := Date secondsToRun(
            testMap foreach(testCaseName, testSlotNames,
                # Depending on the Lobby is kind of wacky, but that's
                # all we can do, since Map only supports string keys.
                testCase := Lobby getSlot(testCaseName)
                testSlotNames foreach(name,
                    testCase setUp
                    exc := try(stopStatus(testCase doString(name)))
                    if(exc, error(testCaseName .. " " .. name, exc), success)
                    testCase tearDown
                )
            )
        )

        printExceptions
        printSummary

        self exceptions
    )

    printExceptions := method(
        "\n" print

        # Printing exceptions in the order they occurred.
        exceptions foreach(exc,
            ("=" repeated(width) .. "\nFAIL: " .. exc at(0) .. \
             "\n" .. "-" repeated(width)) println
            exc at(1) showStack
        )
    )

    printSummary := method(
        // Round run time to milliseconds
        runtimeForDisplay := ((runtime * 1000) - (runtime * 1000 % 1)) / 1000
        "-" repeated(width) println
        ("Ran " .. testCount .. " test" .. if(testCount != 1, "s", "") .. \
         " in " .. runtimeForDisplay .. "s\n") println

        result := if(exceptions isNotEmpty,
            "FAILED (failures #{exceptions size})" interpolate
        ,
            "OK")

        (result .. name alignRight(width - result size) .. "\n") println
    )
)

# A mix-in object, allowing the collectors to run the collected
# tests. Merely a shortcut, since nobody wants to write two lines
# instead of a single word :)
RunnerMixIn := Object clone do(
    run := method(
        TestRunner clone run(prepare)
    )
)

//doc UnitTest setUp Method called prior to each test.
//doc UnitTest tearDown Method called after each test.
//doc UnitTest fail(error) Call to trigger a test failure with a given error message.
UnitTest := Object clone prependProto(RunnerMixIn) do(
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

    prepare := method(Map with(self type, testSlotNames))

    fail := method(error,
        Exception raise(
            if(error, error, "fail")
        )
    )

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
        m ifNil(m = call message)
        if(a != b,
            # Since Message asString is the same as Message code,
            # we don't have to access the latter excplitly inside
            # the interpolated string.
            fail(
                ("`#{ m argAt(0) } != #{ m argAt(1) }` --> " .. \
                 "`#{ a asSimpleString } != #{ b asSimpleString }`") interpolate
            )
        )
    )

    assertNotEquals := method(a, b, m,
        m ifNil(m = call message)
        if(a == b,
            fail(
                ("`#{ m argAt(0) } == #{ m argAt(1) }` --> " .. \
                 "`#{ a asSimpleString } == #{ b asSimpleString }`") interpolate
            )
        )
    )

    assertSame    := method(a, b, assertEquals(a uniqueId, b uniqueId, call message))
    assertNotSame := method(a, b, assertNotEquals(a uniqueId, b uniqueId, call message))
    assertNil     := method(a, assertEquals(a, nil, call message))
    assertNotNil  := method(a, assertNotEquals(a, nil, call message))
    assertTrue    := method(a, assertEquals(a, true, call message))
    assertFalse   := method(a, assertEquals(a, false, call message))

    assertRaisesException := method(
        try(call evalArgAt(0)) ifNil(
            fail("`#{call argAt(0)}` should have raised Exception" interpolate)
        )
    )

    knownBug := method(
        # Probably this should be a special case of assertEquals, so
        # you can be sure that the bug still exists.
        fail("`#{call argAt(0)}` is a known bug" interpolate)
    )

    # Rename this method to assertAlmostEquals?
    assertEqualsWithinDelta := method(expected, actual, delta,
        if(((expected - actual) abs > delta),
            fail("#{expected} expected, but was #{actual} (allowed delta: #{delta})")
        )
    )
)

//metadoc DirectoryCollector category Testing
/*metadoc DirectoryCollector description
An object to collect multiple UnitTests defined in *Test.io files within a given directory (System launchPath directory by default).
*/
DirectoryCollector := Object clone prependProto(RunnerMixIn) do(
    path ::= lazySlot(System launchPath)

//doc DirectoryCollector with(aPath) Returns a new instance with the provided path.
    with := method(path, self clone setPath(path))

    testFiles := method(
        Directory with(path) files select(name endsWithSeq("Test.io"))
    )

    prepare := method(
        # Importing all test files in the set up path to the global namespace.
        testFiles foreach(file,
            # Note: second argument is a label.
            Lobby doString(file contents, file path)
        )

        FileCollector prepare
    )
)

//metadoc FileCollector category Testing
/*metadoc FileCollector description
An object to collect multiple UnitTests defined in the current file.
*/
FileCollector := Object clone prependProto(RunnerMixIn) do(
    prepare := method(
        cases := Map clone

        # Iterating over all of the imported objects and collecting
        # UnitTest instances. Since Block objects doesn't respond
        # correctly to isKindOf, we need to filter out all activatable
        # objects first and only then check for the type (kind).
        Lobby foreachSlot(slotName, slotValue,
            if(getSlot("slotValue") isActivatable not and \
               slotValue isKindOf(UnitTest),
                cases mergeInPlace(slotValue prepare)
            )
        )
        cases
    )
)

# For backward compatibility.
TestSuite := getSlot("DirectoryCollector")
