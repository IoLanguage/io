BoxBodyTest := UnitTest clone do(
	setUp := method(
		self space := ODESimpleSpace clone
		self box := space box(1, 2, 3)

		self world := ODEWorld clone
		self body := world Body clone
	)

	testNoBody := method(
		assertNil(box body)
	)

	testSetBody := method(
		box setBody(body)
		assertSame(box body, body)
	)
)
