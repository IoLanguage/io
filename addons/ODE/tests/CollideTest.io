CollideTest := UnitTest clone do(
	setUp := method(
		self world := ODEWorld clone
		self space := ODESimpleSpace clone

		self body1 := world Body clone setPosition(0.4, 0, 0)
		self box1 := space box(1, 1, 1) setBody(body1)

		self body2 := world Body clone setPosition(-0.4, 0, 0)
		self box2 := space box(1, 1, 1) setBody(body2)
	)

	testBoxCollide := method(
		contacts := box1 collide(box2, 5)
		assertTrue(contacts size != 0)
		contacts foreach(contact,
			assertEquals(contact type, "ODEContact")
		)
	)
)
