addWordyMethods := method(
    statement := call argAt(0)
    rootBuilder := WordyBuilder clone
    while(statement,
        m := statement clone setNextMessage(nil)
        builder := rootBuilder
        while(m and m name != "setSlot",
            builder = builder get(m name)
            m = m attachedMessage
        )
        if(m name == "setSlot",
            name := m argAt(0) cachedResult
            builder = builder get(name) setValue(call sender doMessage(m argAt(1), call sender))
        )
        statement = statement nextMessage
    )

    rootBuilder children foreach(name, builder,
        self setSlot(name, builder handlerMethod(self))
    )

    self
)

/*

Object addWordyMethods(
    should equal := method(value, assertEquals(value, target))
    should not equal := method(value, assertNotEquals(value, target))
    should be := method(value, assertSame(value, target))
    should not be := method(value, assertNotSame(value, target))
    should forward := method(
        name := call message name
        if(target hasSlot(name), assertTrue(call delegateTo(target)))
        # handle is stripping.
    )
    should not forward := method(
        name := call message name
        if(target hasSlot(name), assertFalse(call delegateTo(target) not))
        # handle is stripping.
    )
)

Number addWordyMethods(
    should equals := method
    should not equals := method
    should be := method
    should not be := method
    should be close := method
    should not be close := method
)

nil addWordyMethods(
    should raise := method(e,
        AssertionFailure raise("Expected a '" .. e type .. "' exception.")
    )

    should not raise := nil
)

Exception addWordyMethods(
    should raise := method(e,
        if(target isKindOf(e) not,
            AssertionFailure raise("Unexpected exception '" .. target type .. "' instead of '" .. e type .. "'.", target)
        )
    )

    should not raise := method(
        if(e == nil or target isKindOf(e),
            AssertionFailure raise("Unexpected '" .. target type .. "' exception.", target)
        )
    )
)

List addWordyMethods(
    should contain := method(value, assertTrue(contains(value)))
    should not contain := method(value, assertFalse(contains(value)))
    should have := method(expectedSize, assertEquals(expectedSize, size))
    should not have := method(expectedSize, assertNotEquals(expectedSize, size))
    should have at least := method(minSize, assertTrue(size >= minSize))
    should have at most := method(maxSize, assertTrue(size <= maxSize))
)
*/
