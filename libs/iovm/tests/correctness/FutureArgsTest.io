FutureArgsTest := UnitTest clone do(
	// A future send evaluates its arguments in the sender before queueing
	// the message. That broke targets that read their arguments as code:
	// `list @select(x, x > 1)` raised "Object does not respond to 'x'" (#55).
	// Such messages are now kept as written and run with the sender as
	// their context.

	testSelectAsFuture := method(
		pivot := 3
		lo := list(5, 1, 4, 2) @select(x, x < pivot)
		hi := list(5, 1, 4, 2) @select(x, x > pivot)
		assertEquals(lo size, 2)
		assertEquals(lo, list(1, 2))
		assertEquals(hi, list(5, 4))
	)

	testMapAsFuture := method(
		r := list(1, 2, 3) @map(v, v * 2)
		assertEquals(r at(2), 6)
	)

	testSpecialFormAsFuture := method(
		o := Object clone
		o total := 0
		r := list(1, 2, 3) @foreach(v, o total = o total + v)
		r + 0
		assertEquals(o total, 6)
	)

	testAsyncSendWithCodeArgs := method(
		o := Object clone
		o seen := list()
		list(1, 2, 3) @@foreach(v, o seen append(v))
		3 repeat(yield)
		assertEquals(o seen, list(1, 2, 3))
	)

	testValueArgsStillEvaluatedAtSend := method(
		o := Object clone
		o double := method(n, n * 2)
		futures := list()
		for(i, 1, 3, futures append(o @double(i)))
		assertEquals(futures map(f, f + 0), list(2, 4, 6))
	)

	testArgsAreCodePredicate := method(
		o := Object clone
		o named := method(a, b, a + b)
		assertFalse(o futureArgsAreCode(message(named(1, 2))))
		assertTrue(list() futureArgsAreCode(message(select(x, x > 1))))
		assertTrue(list() futureArgsAreCode(message(foreach(v, v))))
		assertFalse(list() futureArgsAreCode(message(append(1))))
		assertFalse(o futureArgsAreCode(message(noSuchSlot(1))))
	)
)
