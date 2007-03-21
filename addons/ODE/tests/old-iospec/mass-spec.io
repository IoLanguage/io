doFile("context.io")

context("Mass proto",

	setup(
		mass := ODEMass
	)

	specify("should default to a mass of zero",
		mass mass should be(0)
	)

	specify("should default to a center of gravity of zero",
		c := mass centerOfGravity
		c size should be(3)
		c should equal(vector(0, 0, 0))
	)

	specify("should default to an inertia tensor of zero",
		tensor := mass inertiaTensor
		tensor size should be(9)
		tensor should equal(vector(0, 0, 0, 0, 0, 0, 0, 0, 0))
	)

	specify("should default to all parameters being zero",
		mass parameters should equal(vector(0, 0, 0, 0, 0, 0, 0, 0, 0, 0))
	)
)

context("Mass with values after a reset call",
	setup(
		mass := ODEMass clone setMass(3) setCenterOfGravity(1,2,3)
		mass reset
	)

	specify("should have a mass of zero",
		mass mass should be(0)
	)

	specify("should have a centerOfGravity of zero",
		c := mass centerOfGravity
		c size should be(3)
		c should equal(vector(0, 0, 0))
	)

	specify("should have an inertia tensor of zero",
		tensor := mass inertiaTensor
		tensor size should be(9)
		tensor should equal(vector(0, 0, 0, 0, 0, 0, 0, 0, 0))
	)

	specify("should have all parameters equal to zero",
		mass parameters should equal(vector(0, 0, 0, 0, 0, 0, 0, 0, 0, 0))
	)
)

context("Mass instance",

	setup(
		mass := ODEMass clone
	)

	specify("should be able to set the mass",
		mass setMass(13)
		mass mass should be(13)
	)

	specify("should return self from setMass",
		mass setMass(13) should be(mass)
	)

	specify("should be able to set the center of gravity",
		mass setCenterOfGravity(.1,.2,.3)
		mass centerOfGravity should equal(vector(.1,.2,.3))
	)

	specify("should return self from setCenterOfGravity",
		mass setCenterOfGravity(1,2,3) should be(mass)
	)

	specify("should return self from reset",
		mass reset should be(mass)
	)

	specify("should return self from setParameters",
		mass setParameters(.1, .2,.3,.4, .5,.6,.7, .8,.9,.01) should be(mass)
	)
)

context("Mass after setParameters call",
	setup(
		theMass := 1
		cgx := 0.2
		cgy := 0.3
		cgz := 0.4
		I11 := 0.5
		I22 := 0.6
		I33 := 0.7
		I12 := 0.8
		I13 := 0.9
		I23 := 0.01

		mass := ODEMass clone setParameters(theMass, cgx, cgy, cgz, I11, I22, I33, I12, I13, I23)
	)

	specify("should set the mass",
		mass mass should be(theMass)
	)

	specify("should set the center of gravity",
		mass centerOfGravity should equal(vector(cgx, cgy, cgz))
	)

	specify("should set the inertia tensor",
		mass inertiaTensor should equal(vector(I11, I12, I13, I12, I22, I23, I13, I23, I33))
	)

	specify("should return same values from parameters",
		mass parameters should equal(vector(theMass, cgx, cgy, cgz, I11, I22, I33, I12, I13, I23))
	)
)


context("Cloned mass",
	setup(
		original := ODEMass clone
		original setMass(3)
		original setCenterOfGravity(1,2,3)
		mass := original clone
	)

	specify("should have the same mass as the original",
		mass mass should be(original mass)
	)

	specify("should have the same center of gravity as the original",
		mass centerOfGravity should equal(original centerOfGravity)
	)

	specify("should not interfere with mass slot on original",
		mass setMass(4)
		mass mass should be(4)
		original mass should be(3)
	)

	specify("should not be updated when mass slot on original is updated",
		original setMass(4)
		mass mass should be(3)
	)

	specify("should not interfere with centerOfGravity slot on original",
		mass setCenterOfGravity(4,5,6)
		mass centerOfGravity should equal(vector(4,5,6))
		original centerOfGravity should equal(vector(1,2,3))
	)

	specify("should not be updated when centerOfGravity slot on original is updated",
		original setCenterOfGravity(4,5,6)
		mass centerOfGravity should equal(vector(1,2,3))
	)
)

context("sphere density",
	setup(density := 138.2; radius := 1.0; mass := ODEMass clone setSphereDensity(density, radius))
	specify("should have a new mass", mass mass should not be(0))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)

context("sphere mass",
	setup(totalMass := 200; radius := 1.0; mass := ODEMass clone setSphereMass(totalMass, radius))
	specify("should have a new mass", mass mass should be(totalMass))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)

context("capped cylinder density",
	setup(
		density := 138.2; direction := 89; radius := 1.0; length := 31.2
		mass := ODEMass clone setCappedCylinderDensity(density, direction, radius, length)
	)
	specify("should have a new mass", mass mass should not be(0))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)

context("capped cylinder mass",
	setup(
		totalMass := 13; direction := 89; radius := 1.0; length := 31.2
		mass := ODEMass clone setCappedCylinderMass(totalMass, direction, radius, length)
	)
	specify("should have a new mass", mass mass should be(totalMass))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)

context("box density",
	setup(
		density := 138.2; lx := 2.1; ly := 8.1; lz := 7.6
		mass := ODEMass clone setBoxDensity(density, lx, ly, lz)
	)
	specify("should have a new mass", mass mass should not be(0))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)

context("box mass",
	setup(
		totalMass := 13; lx := 2.1; ly := 8.1; lz := 7.6
		mass := ODEMass clone setBoxMass(totalMass, lx, ly, lz)
	)
	specify("should have a new mass", mass mass should be(totalMass))
	specify("should not have changed the center of gravity", mass centerOfGravity should equal(vector(0,0,0)))
	specify("should have a new inertia", mass inertiaTensor should not equal(vector(0,0,0,0,0,0,0,0,0)))
)
