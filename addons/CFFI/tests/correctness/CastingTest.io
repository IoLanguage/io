CFFI

CastingTest := UnitTest clone do(
	testCasting := method(
		appendProto(Types)

		i := Int with(0x00333435)
		ip := i ptr
		cp := ip cast(Char ptr)
		sp := cp cast(CString)

		assertEquals("543", sp value)

		vp := ip cast(Void ptr)
		sp := vp cast(CString)

		assertEquals("543", sp value)

		S := Structure with(	list("x", Int), list("dummy", Byte))
		T := Structure with(	list("a", Char),
					list("b", Char),
					list("c", Char),
					list("d", Char))
		s := S clone setValues(0x31323334, 0)
		vp := s ptr cast(Void ptr)
		svp := vp cast(S ptr)

		assertEquals(0x31323334, svp value x value)

		tp := vp cast(T ptr)

		assertEquals("4", tp value a value)
		assertEquals("3", tp value b value)
		assertEquals("2", tp value c value)
		assertEquals("1", tp value d value)

		tp := svp cast(T ptr)

		assertEquals("4", tp value a value)
		assertEquals("3", tp value b value)
		assertEquals("2", tp value c value)
		assertEquals("1", tp value d value)

		str := tp cast(CString)

		assertEquals("4321", str value)

		charp := tp cast(Char ptr)

		assertEquals("4", charp at(0))
		assertEquals("3", charp at(1))
		assertEquals("2", charp at(2))
		assertEquals("1", charp at(3))

		str := charp cast(CString)
		assertEquals("4321", str value)

		cp := Pointer cast(sp, Char ptr)
		assertEquals("5", cp value)
		
	)
)
