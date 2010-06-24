CFFI

StructureTest := UnitTest clone do(
	testSetValues := method(
		appendProto(Types)
		
		S := Structure with(	list("c", Char),
					list("i", Int),
					list("d", Double),
					list("str", CString))

		c   := Char    clone setValue("A")
		i   := Int     clone setValue(-12345)
		d   := Double  clone setValue(-19256.345)
		str := CString clone setValue("string")

		//This setValues is not valid because "i" is expected as a double.
		//s := S clone setValues(c, i, d, str)

		s := S clone setValues(c, i value, d, str)

		assertEquals(c   value, s c   value)
		assertEquals(i   value, s i   value)
		assertEquals(d   value, s d   value)
		assertEquals(str value, s str value)
	)

	testNestedByValueSetValues := method(
		appendProto(Types)

		S1 := Structure with(	list("c", Char),
					list("i", Int),
					list("d", Double))

		S2 := Structure with(	list("c1", Char),
					list("c2", Char))

		S3 := Structure with(	list("s1", S1),
					list("s2", S2))

		s3 := S3 clone setValues(	S1 clone setValues("t", 876, 567.94),
						S2 clone setValues("U", "p"))

		assertEquals(s3 s1 c  value, "t")
		assertEquals(s3 s1 i  value, 876)
		assertEquals(s3 s1 d  value, 567.94)
		assertEquals(s3 s2 c1 value, "U")
		assertEquals(s3 s2 c2 value, "p")

	)

	testNestedByRefSetValues := method(
		appendProto(Types)
		
		S1 := Structure with(	list("d1", Double),
					list("d2", Double),
					list("c" , Char),
					list("d3", Double),
					list("d4", Double))

		S2 := Structure with(	list("i1", Int),
					list("c1", Char),
					list("s1", CString),
					list("c2", Char))

		S3 := Structure with(	list("s2", S2 ptr),
					list("s1", S1 ptr))

		S4 := Structure with(list("s3", S3 ptr))


		s1 := S1 clone setValues(-92.944, 543.33, "n", -22213.4556, 678.123)

		s2 := S2 clone setValues(1000000, "a", "test", "z")

		/*s3 := S3 clone setValues(	S2 ptr clone setValue(s2),
						S1 ptr clone setValue(s1))

		s4 := S4 clone setValues(S3 ptr clone setValue(s3))*/

		s3 := S3 clone setValues(	s2,
						s1)

		s4 := S4 clone setValues(s3)


		assertEquals(s4 s3 value s1 value d1 value, -92.944)
		assertEquals(s4 s3 value s1 value d2 value, 543.33)
		assertEquals(s4 s3 value s1 value c  value, "n")
		assertEquals(s4 s3 value s1 value d3 value, -22213.4556)
		assertEquals(s4 s3 value s1 value d4 value, 678.123)

		assertEquals(s4 s3 value s2 value i1 value, 1000000)
		assertEquals(s4 s3 value s2 value s1 value, "test")
		assertEquals(s4 s3 value s2 value c1 value, "a")
		assertEquals(s4 s3 value s2 value c2 value, "z")

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

		S := Structure with(	list("c", Char),
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
		
		S := Structure with(	list("s", CString),
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
