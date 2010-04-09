CFFI

// TODO Need to check endianness

UnionTest := UnitTest clone do(
	testSetValues := method(
		appendProto(Types)

		S := Structure with(	list("a", Char),
					list("b", Char),
					list("c", Char),
					list("d", Char))
					
		U := Union with(list("i", Int),
				list("s", S))

		u := U clone

		u i setValue(0x61626364)

		assertEquals((u i value & 0xFF) asCharacter		, u s a value)
		assertEquals(((u i value >> 8 ) & 0xFF) asCharacter	, u s b value)
		assertEquals(((u i value >> 16) & 0xFF) asCharacter	, u s c value)
		assertEquals(((u i value >> 24) & 0xFF) asCharacter	, u s d value)
	
	)

	testNestedByValueSetValues := method(
		appendProto(Types)
		
		S := Structure with(	list("a", Char),
					list("b", Char),
					list("c", Char),
					list("d", Char))

		U1 := Union with(	list("i", Int),
					list("s", S))

		U2 := Union with(	list("u1", U1),
					list("i", Int))

		U3 := Union with(	list("u1", U1),
					list("u2", U2))

		u3 := U3 clone
		u3 u2 u1 s setValues("a", "b", "c", "d")

		assertEquals(u3 u1 i value	, 0x64636261)
		assertEquals(u3 u2 i value	, 0x64636261)
		assertEquals(u3 u2 u1 i value	, 0x64636261)
		assertEquals(u3 u1 s a value	, "a")
		assertEquals(u3 u1 s b value	, "b")
		assertEquals(u3 u1 s c value	, "c")
		assertEquals(u3 u1 s d value	, "d")
		assertEquals(u3 u2 u1 s a value	, "a")
		assertEquals(u3 u2 u1 s b value	, "b")
		assertEquals(u3 u2 u1 s c value	, "c")
		assertEquals(u3 u2 u1 s d value	, "d")

	)

	testPointerToPointers := method(
		appendProto(Types)

		S := Structure with(	list("c", Char),
					list("d", Double),
					list("i", Int))

		s    := S clone setValues("B", -456.78, 45)
		ps   := S ptr clone setValue(s)
		pps  := S ptr ptr clone setValue(ps)
		ppps := S ptr ptr ptr clone setValue(pps)
		
		assertEquals("^{"  ,   ps typeString)
		assertEquals("^^{" ,  pps typeString)
		assertEquals("^^^{", ppps typeString)
		
		assertEquals(s c value,   ps value c value)
		assertEquals(s d value,   ps value d value)
		assertEquals(s i value,   ps value i value)

		assertEquals(s c value,  pps value value c value)
		assertEquals(s d value,  pps value value d value)
		assertEquals(s i value,  pps value value i value)

		assertEquals(s c value, ppps value value value c value)
		assertEquals(s d value, ppps value value value d value)
		assertEquals(s i value, ppps value value value i value)

	)
	
	testAddress := method(
		appendProto(Types)

		S := Union with(	list("c", Char),
					list("d", Double),
					list("i", Int))

		ps   := S ptr clone
		pps  := S ptr ptr clone
		ppps := S ptr ptr ptr clone

		assertEquals(nil,   ps address)
		assertEquals(nil,  pps address)
		assertEquals(nil, ppps address)

		ps   setValue(S clone setValues("B", -456.78, 45))
		pps  setValue(ps)
		ppps setValue(pps)

		assertEquals(ps  address,  pps value address)
		assertEquals(ps  address, ppps value value address)
		assertEquals(pps address, ppps value address)

	)

	testPointedToType := method(
		appendProto(Types)
		
		S := Union with(	list("s", CString),
					list("d", Double),
					list("i", Int))

		1pS := S ptr clone
		2pS := S ptr ptr clone
		3pS := S ptr ptr ptr clone
		4pS := S ptr ptr ptr ptr clone
		5pS := S ptr ptr ptr ptr ptr clone
		
		assertEquals(5pS proto, S ptr ptr ptr ptr ptr)
		assertEquals(5pS pointedToType,                                           4pS proto)
		assertEquals(5pS pointedToType pointedToType,                             3pS proto)
		assertEquals(5pS pointedToType pointedToType pointedToType,               2pS proto)
		assertEquals(5pS pointedToType pointedToType pointedToType pointedToType, 1pS proto)
		
		assertEquals(4pS proto, S ptr ptr ptr ptr)
		assertEquals(4pS pointedToType,                             3pS proto)
		assertEquals(4pS pointedToType pointedToType,               2pS proto)
		assertEquals(4pS pointedToType pointedToType pointedToType, 1pS proto)
		
		assertEquals(3pS proto, S ptr ptr ptr)
		assertEquals(3pS pointedToType,               2pS proto)
		assertEquals(3pS pointedToType pointedToType, 1pS proto)
		
		assertEquals(2pS proto, S ptr ptr)
		assertEquals(2pS pointedToType, 1pS proto)
		
		assertEquals(1pS proto, S ptr)
	)

)
