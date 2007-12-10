
ListTest := UnitTest clone do(
    testAnyOne := method(
		try(Random) ifNonNilEval("      Random addon required for List anyOne" println; return)
		
        a := List clone append("a", "beta", 3)
        assertNil(List anyOne)
        assertTrue(a contains(a anyOne))
        assertTrue(a contains(a anyOne))
    )
    testShuffle := method(
		try(Random) ifNonNilEval("      Random addon required for List shuffle" println; return)
		
        a := List clone append("a", "beta", 3)
        assertTrue(a shuffle contains("a"))
        assertTrue(a shuffle contains("beta"))
        assertTrue(a shuffle contains(3))
    )
)
