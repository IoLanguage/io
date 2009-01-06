SequenceBitTest := UnitTest clone do(

	testBitsWithinFirstByte := method(
		assertEquals(1, 1 asCharacter bitAt(0))
		assertEquals(0, 2 asCharacter bitAt(0))
		assertEquals(1, 2 asCharacter bitAt(1))

		for(i, 0, 7, assertEquals(0, 0 asCharacter bitAt(i)))
		for(i, 0, 7, assertEquals(1, 0xff asCharacter bitAt(i)))
	)

	testBitsWithinSecondByte := method(
		assertEquals(1, ("\0" .. 1 asCharacter) bitAt(8 + 0))
		assertEquals(0, ("\0" .. 2 asCharacter) bitAt(8 + 0))
		assertEquals(1, ("\0" .. 2 asCharacter) bitAt(8 + 1))

		for(i, 0, 7, assertEquals(0, ("\0" .. 0 asCharacter) bitAt(8 + i)))
		for(i, 0, 7, assertEquals(1, ("\0" .. 0xff asCharacter) bitAt(8 + i)))
	)
)
