
FnmatchTest := UnitTest clone do(
	type := "FnmatchTest"
	
	testPatternMatch1 := method(
		r := Fnmatch clone setPattern("*is") matchFor("This")
		assertTrue(r)
	)
	
	testPatternMatch2 := method(
		r := Fnmatch clone setPattern("*is") matchFor("abc def ghi")
		assertFalse(r)
	)
)