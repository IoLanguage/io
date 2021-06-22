
DateTest := UnitTest clone do(

	testBoundaries := method(
		date := Date clone
		date setMonth(12) setDay(31) setHour(23) setMinute(59) setSecond(59)
		assertRaisesException(date setSecond(60))
		assertRaisesException(date setMinute(60))
		assertRaisesException(date setHour(24))
		assertRaisesException(date setDay(32))
		assertRaisesException(date setMonth(13))

		duration := Duration clone
		duration setHours(1)
		date += duration
		assertEquals(0, date hour)
		assertEquals(1, date day)
		assertEquals(1, date month)

		date setSecond(0) setMinute(0) setHour(0) setDay(1) setMonth(1)
		assertRaisesException(date setSecond(-1))
		assertRaisesException(date setMinute(-1))
		assertRaisesException(date setHour(-1))
		assertRaisesException(date setDay(0))
		assertRaisesException(date setMonth(0))
	)

	testFromString := method(
		date := Date fromString("2004/09/06", "%Y/%m/%d")
		assertEquals(2004, date year)
		assertEquals(9, date month)
		assertEquals(6, date day)
	)

	testAsString := method(
		assertEquals("08", Date clone setMonth(8) asString("%m"))
		assertEquals("Aug", Date clone setMonth(8) asString("%b"))
		assertEquals("15:07:13", Date clone setHour(15) setMinute(07) setSecond(13) asString("%X"))
		date := Date clone now
		assertEquals(date year, date asString("%Y") asNumber)
		assertEquals(date month, date asString("%m") asNumber)
		assertEquals(date day, date asString("%d") asNumber)
		assertEquals(date hour, date asString("%H") asNumber)
		assertEquals(date minute, date asString("%M") asNumber)
		assertEquals(date second floor, date asString("%S") asNumber)
		assertEquals(date zone, date asString("%Z"))
	)
	
	testAsSerializationAndFromSerialization := method(
		d := Date clone now
		assertEquals(d, Date clone fromSerialization(d asSerialization))
	)

	testNow := method(

		outputPath := "/tmp/io_date_test"
		datePath := "date"

		isOnWindows := System platform beginsWithSeq("Windows")
		if(isOnWindows,
			writeln("    - SKIPPED: this is not tested on Windows.")
			return
		)

		testString := block(v,
			for(i, 1, 2,
				System system(datePath .. " +\"" .. v .. "\" > " .. outputPath)
				external := File clone open(outputPath) readLine
				internal := Date clone now asString(v)
				if(external == internal, break)
			)
			assertEquals(external, internal)
		)

		testString(Date format)
		testString("%b")
		testString("%X")
		//testString("%c") #PortableStrptime puts zeros in front of single digit month numbers
		//testString("%x") #%x is locale dependent
		testString("%y")
	)
)

