CFFI

ComplexTest := UnitTest clone do(

	testArrayOfStructures := method(
		appendProto(Types)

		S1 := Structure with(	list("c", Char),
					list("i", Int),
					list("d", Double))

		S2 := Structure with(	list("c1", Char),
					list("c2", Char))

		S3 := Structure with(	list("s1", S1),
					list("s2", S2))

		AI := Array with(S3, 10)

		a := AI clone
		for(i, 0, 9, 1,
			a atPut(i, S3 clone setValues(	S1 clone setValues(("t" at(0) + i) asCharacter, 876 + i, 567.94 + i),
							S2 clone setValues(("U" at(0) + i) asCharacter, ("p" at(0) + i) asCharacter)) )

		)

		for(i, 0, 9, 1,
			s := a at(i)

			assertEquals(s s1 c  value, ("t" at(0) + i) asCharacter)
			assertEquals(s s1 i  value, 876 + i)
			assertEquals(s s1 d  value, 567.94 + i)
			assertEquals(s s2 c1 value, ("U" at(0) + i) asCharacter)
			assertEquals(s s2 c2 value, ("p" at(0) + i) asCharacter)
		)

	)

	testArrayOfStructurePointers := method(
		appendProto(Types)

		S1 := Structure with(	list("c", Char),
					list("i", Int),
					list("d", Double))

		S2 := Structure with(	list("c1", Char),
					list("c2", Char))

		S3 := Structure with(	list("s1", S1),
					list("s2", S2))

		AS3 := Array with(S3 ptr, 10)

		as3 := AS3 clone
		for(i, 0, 9, 1,
			as3 atPut(i,	S3 with(S1 with(("t" at(0) + i) asCharacter, 876 + i, 567.94 + i),
					S2 with(("U" at(0) + i) asCharacter, ("p" at(0) + i) asCharacter))
			)
			

		)

		for(i, 0, 9, 1,
			s := as3 at(i) value

			assertEquals(s s1 c  value, ("t" at(0) + i) asCharacter)
			assertEquals(s s1 i  value, 876 + i)
			assertEquals(s s1 d  value, 567.94 + i)
			assertEquals(s s2 c1 value, ("U" at(0) + i) asCharacter)
			assertEquals(s s2 c2 value, ("p" at(0) + i) asCharacter)
		)

	)


	testStructureOfArrays := method(
		appendProto(Types)

		S1 := Structure with(	list("i", Int),
					list("d", Double))

		AS1 := Array with(S1, 10)

		S2 := Structure with(	list("c1", Char),
					list("as1", AS1),
					list("c2", Char),
					list("as2", AS1))

		S3 := Structure with(	list("s1", S1),
					list("s2", S2))

		a := Array with(S3, 10) clone
		for(i, 0, 9, 1,
			s2 := S2 clone

			s2 c1 setValue(("a" at(0) + i) asCharacter)
			s2 c2 setValue(("z" at(0) - i) asCharacter)

			for(j, 0, 9, 1,
				s2 as1 atPut( j, S1 with( (i * 10) + j, (i * 10) + (j + 10) ) )
				s2 as2 atPut( j, S1 with( (i * 10) + (j + 10), (i * 10) + j ) )
			)

			a atPut(i, S3 with(S1 with(7831 + i, 2678.878 + i), s2))

		)

		for(i, 0, 9, 1,
			s := a at(i)

			assertEquals(s s1 i  value, 7831 + i)
			assertEquals(s s1 d  value, 2678.878 + i)

			assertEquals(s s2 c1 value, ("a" at(0) + i) asCharacter)
			assertEquals(s s2 c2 value, ("z" at(0) - i) asCharacter)

			as1 := s s2 as1
			as2 := s s2 as2
			for(j, 0, 9, 1,
				assertEquals( as1 at(j) i value, (i * 10) + j )
				assertEquals( as1 at(j) d value, (i * 10) + (j + 10) )

				assertEquals( as2 at(j) i value, (i * 10) + (j + 10) )
				assertEquals( as2 at(j) d value, (i * 10) + j )
			)
		)

	)
)
