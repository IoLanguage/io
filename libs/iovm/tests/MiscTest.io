
MiscTest := UnitTest clone do(
	testAssignmentMultiplicationBug := method(
		e := try (r := 3 * 4)
		e catch ( Exception, fail )
	)

	testActivate := method(
		a := Object clone
		a activate := method(activateWorks = true)
		a setIsActivatable(true)

		activateWorks := false
		a

		assertTrue(activateWorks)
	)
)
