doFile("context.io")

context("mass and body",

	setup(
		world := ODEWorld clone
		body := world Body clone
		mass := ODEMass clone setSphereMass(10, 1)
	)

	specify("should be able to set the mass of the body",
		body setMass(mass)
	)

	specify("should body setMass should return body",
		body setMass(mass) should be(body)
	)
)

context("body with mass",
	setup(
		world := ODEWorld clone
		body := world Body clone
		mass := ODEMass clone setSphereMass(10, 1)
		body setMass(mass)
	)

	specify("should return a mass equal to the original mass",
		body mass should equal(mass)
	)
)

context("a clean body",
	setup(
		body := ODEWorld clone Body clone
	)

	specify("should be able to set the position",
		body setPosition(1.4, 2, 37)
	)

	specify("should return self when sent setPosition",
		body setPosition(1.4, 2, 37) should be(body)
	)

	specify("should be able to add force", body addForce(1,2,3))
	specify("should return self when sent addForce", body addForce(1,2,3) should be(body))
)

context("body and position",
	setup(
		body := ODEWorld clone Body clone
		x := 31.2
		y := 8.4
		z := 19
		body setPosition(x, y, z)
	)

	specify("should be able to set the position",
		body position should equal(vector(x, y, z))
	)
)

context("body and force",
	setup(
		body := ODEWorld clone Body clone
		x := 31.2
		y := 8.4
		z := 19
		body setForce(x, y, z)
	)

	specify("should set force correctly",
		body force should equal(vector(x, y, z))
	)

	specify("should add forces",
		body addForce(x, y, z) force should equal(vector(x, y, z) * 2)
	)
)


writeln("Done.")
