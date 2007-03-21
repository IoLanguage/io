CollisionCallbackTest := UnitTest clone do(
	setUp := method(
		self world := ODEWorld clone
		self space := ODESimpleSpace clone

		self body1 := world Body clone
		self box1 := space box(1, 1, 1) setBody(body1)

		self body2 := world Body clone
		self box2 := space box(1, 1, 1) setBody(body2)
	)

	testNearCollision := method(
		box1 := box1
		box2 := box2
		nearCollision := method(o1, o2,
			if(box1 == o1,
				assertSame(o1, box1)
				assertSame(o2, box2)
			,
				assertSame(o2, box1)
				assertSame(o1, box2)
			)
		)
		space collide(thisContext, message(nearCollision))
	)
)
