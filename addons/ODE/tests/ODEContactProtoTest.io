ODEContactProtoTest := UnitTest clone do(
	setUp := method(
		self contact := ODEContact
	)

	testPosition := method(
		# should default to zero
		assertTrue(contact position isZero)

		# should be settable
		contact setPosition(1,2,3)
		assertEquals(contact position, vector(1,2,3))
	)

	testNormal := method(
		# should default to zero
		assertTrue(contact normal isZero)

		# should be settable
		contact setNormal(1,2,3)
		assertEquals(contact normal, vector(1,2,3))
	)

	testDepth := method(
		# should default to zero
		assertEquals(contact depth, 0)

		# should be settable
		contact setDepth(13)
		assertEquals(contact depth, 13)
	)

	testGeoms := method(
		b := ODESimpleSpace box(1,1,1,1)

		# geom1
		assertNil(contact geom1)
		
		contact setGeom1(b)
		assertSame(contact geom1, b)

		contact setGeom1(nil)
		assertNil(contact geom1)

		# geom2
		assertNil(contact geom2)
		
		contact setGeom2(b)
		assertSame(contact geom2, b)

		contact setGeom2(nil)
		assertNil(contact geom2)
	)

	testSides := method(
		# side1
		assertEquals(contact side1, 0)

		contact setSide1(13)
		assertEquals(contact side1, 13)

		# side2
		assertEquals(contact side2, 0)

		contact setSide2(13)
		assertEquals(contact side2, 13)
	)

	testFrictionDirection := method(
		# should default to zero
		assertTrue(contact frictionDirection isZero)

		# should not enable by defualt
		assertFalse(contact frictionDirectionEnabled)

		# should be settable
		contact setFrictionDirection(1,2,3)
		assertEquals(contact frictionDirection, vector(1,2,3))

		# should enable when set
		contact setFrictionDirection(1,2,3)
		assertTrue(contact frictionDirectionEnabled)

		# should disable when disabled
		contact setFrictionDirection(1,2,3)
		contact disableFrictionDirection
		assertFalse(contact frictionDirectionEnabled)
	)

	testMu := method(
		# should default to 0
		assertEquals(contact mu, 0)

		# should be settable
		contact setMu(13)
		assertEquals(contact mu, 13)
	)

	testMu2 := method(surfaceVariable("mu2"))
	testBounce := method(surfaceVariable("bounce"))
	testSoftErp := method(surfaceVariable("softErp"))
	testSoftCfm := method(surfaceVariable("softCfm"))
	testMotion1 := method(surfaceVariable("motion1"))
	testMotion2 := method(surfaceVariable("motion2"))
	testSlip1 := method(surfaceVariable("slip1"))
	testSlip2 := method(surfaceVariable("slip2"))

	testBounceVelocity := method(
		# should default to 0
		assertEquals(contact bounceVelocity, 0)

		# should be settable
		contact setBounceVelocity(13)
		assertEquals(contact bounceVelocity, 13)
	)

	surfaceVariable := method(name,
		cappedName := name asMutable capitalize

		# should default to 0
		assertEquals(contact doString(name), 0)

		# should default to disabled
		assertFalse(contact doString(name .. "Enabled"))

		# should be settable
		contact doMessage(Message clone setName("set" .. cappedName) appendCachedArg(13))
		assertEquals(contact doString(name), 13)

		# should enable when set
		contact doMessage(Message clone setName("set" .. cappedName) appendCachedArg(13))
		assertTrue(contact doString(name .. "Enabled"))

		# should disable
		contact doMessage(Message clone setName("disable" .. cappedName))
		assertFalse(contact doString(name .. "Enabled"))

	)
)
