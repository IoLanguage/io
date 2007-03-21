LoopBreakingTest := UnitTest clone do(
	setUp := method(
		List foreach2 := method(
			slotName := call argAt(0) name
			body := call argAt(1)

			self foreach(v,
				call sender setSlot(slotName, v)
				ss := stopStatus(result := call sender doMessage(body, call sender))
				if(ss stopLooping, break)
			)

			if(ss isReturn,
				call setStopStatus(ss)
			)
			result
		)
	)

	tearDown := method(
		List removeSlot("foreach2")
	)


	testBreak := method(
		a := list; list(1,2,3,4,5) foreach(v, a append(v); if(v >= 3, break))
		b := list; list(1,2,3,4,5) foreach2(v, b append(v); if(v >= 3, break))
		assertEquals(a, b)
	)

	testContinue := method(
		a := list; list(1,2,3,4,5) foreach(v, a append(v); if(v >= 3, continue); a append(v))
		b := list; list(1,2,3,4,5) foreach2(v, b append(v); if(v >= 3, continue); b append(v))
		assertEquals(a, b)
	)

	testReturn := method(
		fa := method(r := list; list(1,2,3,4,5) foreach(v, r append(v); if(v >= 3, return r); r append(v)); r append("bad"); r)
		fb := method(r := list; list(1,2,3,4,5) foreach2(v, r append(v); if(v >= 3, return r); r append(v)); r append("bad"); r)
		a := fa
		b := fb
		assertEquals(a, b)
	)
)
