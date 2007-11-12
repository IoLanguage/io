setSlot("ODE", ODEWorld)

ODEMass do(
	slotDescriptionMap := method(
		m := super(slotDescriptionMap)
		m atPut("mass", mass asSimpleString)
		m atPut("centerOfGravity", centerOfGravity asSimpleString)
		m atPut("inertiaTensor", inertiaTensor asSimpleString)
		m
	)

	setSlot("==",
		method(o,
			(mass == o mass) and (centerOfGravity == o centerOfGravity) and (inertiaTensor == o inertiaTensor)
		)
	)

	asSimpleString := method(
		s := Sequence clone appendSeq("ODEMass mass(", mass asString, ") I(")
		inertiaTensor := inertiaTensor
		for(i, 0, 9, 
			if(i != 0, s appendSeq(", "))
			s appendSeq(inertiaTensor at(i) asString)
		)
		s appendSeq(")")
		
		centerOfGravity := centerOfGravity
		if(centerOfGravity isZero not,
			s appendSeq(" centerOfGravity(", centerOfGravity at(0) asString, " ", centerOfGravity at(1) asString, " ", centerOfGravity at(2) asString, ")")
		)
		s
	)
)

ODEContact do(
	slotDescriptionMap := method(
		m := super(slotDescriptionMap)
		m atPut("position", position asSimpleString)
		m atPut("normal", normal asSimpleString)
		m atPut("depth", depth asSimpleString)
		m atPut("geom1", geom1 asSimpleString)
		m atPut("geom2", geom2 asSimpleString)
		m atPut("side1", side1 asSimpleString)
		m atPut("side2", side2 asSimpleString)
		m atPut("mu", mu asSimpleString)

		possiblyDisabled := method(name,
			m atPut(name,
				if(doString(name .. "Enabled"),
					doString(name) asSimpleString
				,
					"disabled"
				)
			)
		)

		possiblyDisabled("frictionDirection")
		possiblyDisabled("mu2")
		possiblyDisabled("bounce")

		m atPut("bounceVelocity",
			if(bounceEnabled,
				bounceVelocity asSimpleString
			,
				"disabled"
			)
		)

		possiblyDisabled("softErp")
		possiblyDisabled("softCfm")
		possiblyDisabled("motion1")
		possiblyDisabled("motion2")
		possiblyDisabled("slip1")
		possiblyDisabled("slip2")
		m
	)
)

ODEWorld do(
	asSimpleString := method(
		s := Sequence clone
		s appendSeq(self type, "_", worldId)
		if(worldId != 0,
			g := gravity
			s appendSeq(" gravity(", g x, ", ", g y, ", ", g z, ")")
			s appendSeq(" erp(", erp, ")")
			s appendSeq(" cfm(", cfm, ")")
		)
		s
	)
)

ODEPlane do(
	body := nil
)

addExtraSlotDescriptions := method(obj, idName, evalSlotList,
        meth := call activated getSlot("slotDescriptionMapTemplate") clone
        getSlot("meth") idName := idName
        getSlot("meth") evalSlotList := evalSlotList append(idName)
        obj slotDescriptionMap := getSlot("meth")
) do(
        slotDescriptionMapTemplate := method(
                m := super(slotDescriptionMap)
                if(doString(call activated idName) != 0,
                        call activated evalSlotList foreach(slot,
                                m atPut(slot, doString(slot) asSimpleString)
                        )
                )
                m
        )
)

addExtraSlotDescriptions(ODEWorld, "worldId", list("gravity", "erp", "cfm"))
addExtraSlotDescriptions(ODEBody, "bodyId", list("world", "position", "mass", "force", "torque", "rotation", "quaternion"))
addExtraSlotDescriptions(ODEJointGroup, "jointGroupId", list())

addExtraSlotDescriptions(ODEBall, "jointId", list("anchor", "anchor2"))
addExtraSlotDescriptions(ODEFixed, "jointId", list())
addExtraSlotDescriptions(ODEHinge, "jointId", list("lowStop", "highStop", "velocity", "maxForce", "fudgeFactor", "bounce", "cfm", "stopErp", "stopCfm", "anchor", "anchor2", "axis", "angle", "angleRate"))
addExtraSlotDescriptions(ODEHinge2, "jointId", list("lowStop", "highStop", "velocity", "maxForce", "fudgeFactor", "bounce", "cfm", "stopErp", "stopCfm", "anchor", "anchor2", "axis1", "axis2", "angle1", "angle1Rate", "angle2Rate"))
addExtraSlotDescriptions(ODESlider, "jointId", list("lowStop", "highStop", "velocity", "maxForce", "fudgeFactor", "bounce", "cfm", "stopErp", "stopCfm", "axis", "position", "positionRate"))
addExtraSlotDescriptions(ODEUniversal, "jointId", list("lowStop", "highStop", "velocity", "maxForce", "fudgeFactor", "bounce", "cfm", "stopErp", "stopCfm", "anchor", "anchor2", "axis1", "axis2", "angle1", "angle2", "angle1Rate", "angle2Rate"))

addExtraSlotDescriptions(ODESimpleSpace, "spaceId", list())
addExtraSlotDescriptions(ODEPlane, "geomId", list("params"))
addExtraSlotDescriptions(ODEBox, "geomId", list("body", "lengths"))

removeSlot("addExtraSlotDescriptions")
