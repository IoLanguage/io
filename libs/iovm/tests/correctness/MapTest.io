
MapTest := UnitTest clone do(
	setUp := method(
		super(setUp)
		self exampleMap := Map clone atPut("a", "alpha")  atPut("b", "beta")
	)

	testClone := method(
		assertNotSame(Map, Map clone)
		assertEquals(2, exampleMap size)
		clonedMap := exampleMap clone
		assertNotSame(clonedMap, exampleMap)
		assertEquals(2, clonedMap size)
		assertEquals(exampleMap at("a"), clonedMap at("a"))
		assertEquals(exampleMap at("b"), clonedMap at("b"))
	)

	testEmpty := method(
		exampleMap empty
		assertEquals(0, exampleMap size)
	)

	testAt := method(
		assertRaisesException(exampleMap at())
		assertRaisesException(exampleMap at(nil))
		assertEquals("beta", exampleMap at("b"))
		assertEquals("alpha", exampleMap at("a"))
		assertNil(exampleMap at("c"))
		assertNil(exampleMap at("alpha"))
	)

	testAtPut := method(
		assertRaisesException(exampleMap atPut(nil, "thirtythree"))
		assertRaisesException(exampleMap atPut(33))
		assertRaisesException(exampleMap atPut(33, "thirtythree"))
		exampleMap atPut("a", 88)
		exampleMap atPut("blah", "fasel")
		exampleMap atPut("nil", nil)
		assertEquals("fasel", exampleMap at("blah"))
		assertEquals(88, exampleMap at("a"))
		assertNil(exampleMap at("nil"))
	)

	testAtIfAbsentPut := method(
		assertRaisesException(exampleMap atIfAbsentPut(nil, "thirtythree"))
		assertRaisesException(exampleMap atIfAbsentPut(33))
		assertRaisesException(exampleMap atIfAbsentPut(33, "thirtythree"))
		exampleMap atIfAbsentPut("a", 88)
		exampleMap atIfAbsentPut("blah", "fasel")
		assertEquals("fasel", exampleMap at("blah"))
		assertEquals("alpha", exampleMap at("a"))
	)

	testsize := method(
		assertEquals(0, Map size)
		assertEquals(2, exampleMap size)
		exampleMap atPut("blah", "fasel")
		assertEquals(3, exampleMap size)
	)

	testKeys := method(
		assertEquals(List type, exampleMap keys type)
		assertEquals(0, Map keys size)
		keys := exampleMap keys
		assertEquals(2, keys size)
		assertTrue(keys contains("a"))
		assertTrue(keys contains("b"))
	)

	testValues := method(
		assertEquals(List type, exampleMap values type)
		assertEquals(0, Map values size)
		exampleMap atPut("nil", nil)
		values := exampleMap values
		assertEquals(3, values size)
		assertTrue(values contains("alpha"))
		assertTrue(values contains("beta"))
		assertTrue(values contains(nil))
	)

	testForeach := method(
		s := ""
		exampleMap foreach(key, value,
			//writeln("key = ", key)
			//writeln("value = ", value)
			s = s .. key .. value
		)
		//writeln("\ns = [", s, "]")
		assertTrue(s == "aalphabbeta" or s == "bbetaaalpha" )
	)

	testHasKey := method(
		assertRaisesException(exampleMap hasKey())
		assertRaisesException(exampleMap hasKey(nil))
		assertTrue(exampleMap hasKey("a"))
		assertTrue(exampleMap hasKey("b"))
		assertFalse(exampleMap hasKey("c"))
	)

	testHasValue := method(
		assertTrue(exampleMap hasValue("alpha"))
		assertTrue(exampleMap hasValue("beta"))
		assertFalse(exampleMap hasValue("whatever"))
	)

	testRemoveAt := method(
		assertRaisesException(exampleMap removeAt())
		assertRaisesException(exampleMap removeAt(nil))
		exampleMap removeAt("c")
		assertEquals(2, exampleMap size)
		exampleMap removeAt("a")
		assertEquals(1, exampleMap size)
		//assertFalse(exampleMap hasKey("a"))
	)

	testAsList := method(
		m := Map clone atPut("a", 1) atPut("b", 2) atPut("c", 3) atPut("d", 4)
		l := m asList
		assertEquals(4, l size)

		l foreach(pair,
			key := pair at(0)
			value := pair at(1)
			assertEquals(2, pair size)

			assertTrue(m hasKey(key))
			assertEquals(m at(key), value)

			m removeAt(key)
		)

		assertEquals(0, m size)
	)
	
	testMergeInPlace := method(
		m := Map clone atPut("a", 0) atPut("b", 1)
		m mergeInPlace(Map clone atPut("a", -1) atPut("c", 2))
		
		assertEquals(3, m size)
		assertEquals(-1, m at("a"))
		assertEquals(1, m at("b"))
		assertEquals(2, m at("c"))
	)
	
	testMerge := method(
		m := Map clone atPut("a", 0) atPut("b", 1)
		m2 := m merge(Map clone atPut("a", -1) atPut("c", 2))
		
		assertEquals(3, m2 size)
		assertEquals(-1, m2 at("a"))
		assertEquals(1, m2 at("b"))
		assertEquals(2, m2 at("c"))
		
		assertEquals(2, m size)
		assertEquals(0, m at("a"))
		assertFalse(m hasKey("c"))
	)
)
