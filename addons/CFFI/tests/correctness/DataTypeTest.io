/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

CFFI

DataTypeTest := UnitTest clone do(
	testValue := method(
		appendProto(Types)
		
		assertEquals("c", Char setValue("c") value)
		assertEquals("c", UChar setValue("c") value)
		assertEquals(-15, Short setValue(-15) value)
		assertEquals(15, UShort setValue(15) value)
		assertEquals(-187, Int setValue(-187) value)
		assertEquals(187, UInt setValue(187) value)
		assertEquals(-9473, Long setValue(-9473) value)
		assertEquals(9473, ULong setValue(9473) value)
		assertEquals(-7.65434, Double setValue(-7.65434) value)
		assertEquals("abc", CString setValue("abc") value)
		assertEquals(-100000000000, LongLong setValue(-100000000000) value)
		assertEquals(13334465676550000, ULongLong setValue(13334465676550000) value)
	)
)
