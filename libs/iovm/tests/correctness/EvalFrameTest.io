EvalFrameTest := UnitTest clone do(
	testCurrentFrame := method(
		f := Coroutine currentCoroutine currentFrame
		assertNotNil(f)
		assertTrue(f depth > 0)
	)

	testFrameState := method(
		f := Coroutine currentCoroutine currentFrame
		assertTrue(f state isKindOf(Sequence))
	)

	testFrameParent := method(
		f := Coroutine currentCoroutine currentFrame
		// There should always be a parent frame (we're inside a method)
		assertNotNil(f parent)
		assertTrue(f parent != nil)
	)

	testFrameDepth := method(
		f := Coroutine currentCoroutine currentFrame
		d := f depth
		assertTrue(d >= 3)  // at least: doString, method activation, this chain
	)

	testFrameDescription := method(
		f := Coroutine currentCoroutine currentFrame
		desc := f description
		assertTrue(desc isKindOf(Sequence))
		assertTrue(desc size > 0)
	)

	testFrameWalk := method(
		// Walk the full frame stack and verify it terminates
		f := Coroutine currentCoroutine currentFrame
		count := 0
		while(f != nil,
			count = count + 1
			f = f parent
		)
		assertTrue(count > 0)
	)

	testFrameCallInMethod := method(
		helper := method(
			f := Coroutine currentCoroutine currentFrame
			// Walk up to find the frame with a call object
			while(f != nil,
				if(f call != nil, return f call)
				f = f parent
			)
			return nil
		)
		c := helper
		assertNotNil(c)
		assertEquals("Call", c type)
	)

	testFrameLocals := method(
		f := Coroutine currentCoroutine currentFrame
		// Walk to find a frame with non-nil locals
		while(f != nil,
			if(f locals != nil, break)
			f = f parent
		)
		assertNotNil(f)
		assertNotNil(f locals)
	)

	testFrameTarget := method(
		f := Coroutine currentCoroutine currentFrame
		while(f != nil,
			if(f target != nil, break)
			f = f parent
		)
		assertNotNil(f)
		assertNotNil(f target)
	)
)
