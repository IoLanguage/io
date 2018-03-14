Range
#list(asList, at, contains, first, foreach, index, indexOf, last, map, next, previous, rewind, select, setRange, slice, value)

RangeTest := UnitTest clone do(
    setUp := method(
        super(setUp)
		self exampleRange := Range clone setRange(5,10)
		self exampleRangeWithCustomStep := Range clone setRange(5,10,2)
		self decreasingExampleRange := Range clone setRange(10,5)
		self decreasingExampleRangeWithCustomStep := Range clone setRange(10,5,-2)
	)

    testasList := method(
        assertEquals(list(5,6,7,8,9,10), exampleRange asList)
        assertEquals(list(5,7,9), exampleRangeWithCustomStep asList)
        assertEquals(list(10,9,8,7,6,5), decreasingExampleRange asList)
        assertEquals(list(10,8,6), decreasingExampleRangeWithCustomStep asList)
    )

      # Checking for valid index and out of bounds
      #
    testat := method(
    assertEquals(exampleRangeWithCustomStep at(2), 9)
        assertRaisesException(exampleRangeWithCustomStep at(6))
        assertRaisesException(exampleRangeWithCustomStep at(-1))

        assertEquals(decreasingExampleRangeWithCustomStep at(2), 6)
        assertRaisesException(decreasingExampleRangeWithCustomStep at(6))
        assertRaisesException(decreasingExampleRangeWithCustomStep at(-1))
    )

	# Should work correct with custom step ranges
	#
    testcontains := method(
        assertTrue(exampleRangeWithCustomStep contains(7))
        assertTrue(exampleRangeWithCustomStep contains(9))
        assertFalse(exampleRangeWithCustomStep contains(10))

        assertTrue(decreasingExampleRangeWithCustomStep contains(10))
        assertTrue(decreasingExampleRangeWithCustomStep contains(8))
        assertFalse(decreasingExampleRangeWithCustomStep contains(5))
	)

	# Returns first item and sets index to 0
	#
    testfirst := method(
        exampleRangeWithCustomStep next
        assertEquals(5, exampleRangeWithCustomStep first)
        assertEquals(0, exampleRangeWithCustomStep index)

        decreasingExampleRangeWithCustomStep next
        assertEquals(10, decreasingExampleRangeWithCustomStep first)
        assertEquals(0, decreasingExampleRangeWithCustomStep index)
    )

    # Returns valid value and index at each step
    # Recovers previous state 
    # 
    testforeach_withTwoArgs := method(
        index := 0
        listed := exampleRangeWithCustomStep asList
        exampleRangeWithCustomStep foreach(i,v,
            assertEquals(i, index)
            assertEquals(v, listed at(index))
            index = index + 1
        )
        assertEquals(5, exampleRangeWithCustomStep ?value)
        assertEquals(0, exampleRangeWithCustomStep ?index)
    
        index = 0;
        listed := decreasingExampleRangeWithCustomStep asList
        decreasingExampleRangeWithCustomStep foreach(i,v,
            assertEquals(i, index)
            assertEquals(v, listed at(index))
            index = index + 1
        )
       	assertEquals(10, decreasingExampleRangeWithCustomStep ?value)
   	    assertEquals(0, decreasingExampleRangeWithCustomStep ?index)
    )

    testforeach_withOneArg := method(
        index := 0
        listed := exampleRangeWithCustomStep asList
        exampleRangeWithCustomStep foreach(v,
            assertEquals(v, listed at(index))
            index = index + 1
        )
        assertEquals(5, exampleRangeWithCustomStep ?value)
        assertEquals(0, exampleRangeWithCustomStep ?index)
    
        index = 0;
        listed := decreasingExampleRangeWithCustomStep asList
        decreasingExampleRangeWithCustomStep foreach(v,
            assertEquals(v, listed at(index))
            index = index + 1
        )
   	    assertEquals(10, decreasingExampleRangeWithCustomStep ?value)
   	    assertEquals(0, decreasingExampleRangeWithCustomStep ?index)
    )
  
    testforeach_withoutArgs := method(
        index := 0
        listed := exampleRangeWithCustomStep asList
        lst := List clone
        exampleRangeWithCustomStep foreach(v,
            index = index + 1
        )
        assertEquals(3, index)
        assertEquals(5, exampleRangeWithCustomStep ?value)
        assertEquals(0, exampleRangeWithCustomStep ?index)
    
        index = 0;
        listed := decreasingExampleRangeWithCustomStep asList
        decreasingExampleRangeWithCustomStep foreach(v,
            index = index + 1
        )
        assertEquals(3, index)
   	    assertEquals(10, decreasingExampleRangeWithCustomStep ?value)
       	assertEquals(0, decreasingExampleRangeWithCustomStep ?index)
	)

    # Correctness of index at range borders
    #
    testindex := method(
        exampleRangeWithCustomStep last
        assertEquals(2, exampleRangeWithCustomStep index)
        exampleRangeWithCustomStep next
        assertEquals(2, exampleRangeWithCustomStep index)
        exampleRangeWithCustomStep first
        assertEquals(0, exampleRangeWithCustomStep index)
        assertEquals(1, exampleRangeWithCustomStep next index)

        decreasingExampleRangeWithCustomStep last
        assertEquals(2, decreasingExampleRangeWithCustomStep index)
        decreasingExampleRangeWithCustomStep next
        assertEquals(2, decreasingExampleRangeWithCustomStep index)
        decreasingExampleRangeWithCustomStep first
        assertEquals(0, decreasingExampleRangeWithCustomStep index)
        assertEquals(1, decreasingExampleRangeWithCustomStep next index)
    )

    testindexOf := method(
        assertEquals(1, exampleRangeWithCustomStep indexOf(7))
        assertNil(exampleRangeWithCustomStep indexOf(10))
        
        assertNil(decreasingExampleRangeWithCustomStep indexOf(7))
        assertEquals(1, decreasingExampleRangeWithCustomStep indexOf(8))
    )

    # Should work correctly with custom step ranges
    #
    testlast := method(
        assertEquals(9, exampleRangeWithCustomStep last)
        assertEquals(6, decreasingExampleRangeWithCustomStep last)
    )

	# Map with 1,2 or 3 arguments
	#
    testmap := method(
	   	assertEquals(list(10,14,18), exampleRangeWithCustomStep map(*2))
	   	assertEquals(list(25,49,81), exampleRangeWithCustomStep map(v, v*v))
	   	assertEquals(list(0,7,18), exampleRangeWithCustomStep map(i,v, i*v))

	   	assertEquals(list(20,16,12), decreasingExampleRangeWithCustomStep map(*2))
	   	assertEquals(list(100,64,36), decreasingExampleRangeWithCustomStep map(v, v*v))
	   	assertEquals(list(0,8,12), decreasingExampleRangeWithCustomStep map(i,v, i*v))
    )

    # Returns valid next value or nil if the current item is last
    #
    testnext := method(
        assertEquals(7, exampleRangeWithCustomStep next ?value)
        assertEquals(9, exampleRangeWithCustomStep next ?value)
        assertNil(exampleRangeWithCustomStep next ?value)
    )
  
    # Checking bounds and step
    #
    testsetRange := method(
        assertEquals(5, exampleRange first)
        assertEquals(0, exampleRange index)
        assertEquals(6, exampleRange next ?value)
	    assertEquals(1, exampleRange index)
	    assertEquals(10, exampleRange last)
	    assertEquals(5, exampleRange index)
    )

    testsetRange_withStep := method(
        assertEquals(5, exampleRangeWithCustomStep first)
        assertEquals(0, exampleRangeWithCustomStep index)
        assertEquals(7, exampleRangeWithCustomStep next ?value)
        assertEquals(1, exampleRangeWithCustomStep index)
        # if you got error here, the problem may be with 'last' not with setRange itself
        assertEquals(9, exampleRangeWithCustomStep last) 
        assertEquals(2, exampleRangeWithCustomStep index)
    )
  
    testsetRange_decreasing := method(
        assertEquals(10, decreasingExampleRange first)
        assertEquals(0, decreasingExampleRange index)
        assertEquals(9, decreasingExampleRange next ?value)
        assertEquals(5, decreasingExampleRange last)
        assertEquals(5, decreasingExampleRange index)
    )
  
    estsetRange_decreasingWithStep := method(
        assertEquals(10, decreasingExampleRangeWithCustomStep first)
        assertEquals(0, decreasingExampleRangeWithCustomStep index)
        assertEquals(8,  decreasingExampleRangeWithCustomStep next ?value)
        # if you got error here, the problem may be with 'last' not with setRange itself
        assertEquals(6, decreasingExampleRangeWithCustomStep last)
        assertEquals(2, decreasingExampleRangeWithCustomStep index)
    )

    testValue := method(
        assertEquals(exampleRange value, 5)
    )
)

