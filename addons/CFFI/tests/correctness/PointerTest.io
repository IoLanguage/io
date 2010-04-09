/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

CFFI

PointerTest := UnitTest clone do(
	testValueWithSignedInteger := method(
		appendProto(Types)
		
		n := -986
		assertEquals(n, Short ptr clone setValue(Short clone setValue(n)) value)
		assertEquals(n, SShort ptr clone setValue(SShort clone setValue(n)) value)
		
		assertEquals(n, Long ptr clone setValue(Long clone setValue(n)) value)
		assertEquals(n, SLong ptr clone setValue(SLong clone setValue(n)) value)
		
		// I really have no idea why these don't pass with UnitTest, both work
		// fine for me when testing from the CLI
		//  - The problem was the "Collecter collect"s in UnitTest.io.  Still
		//    deciding if I should submit a patch "fixing" UnitTest
		assertEquals(n, Int ptr clone setValue(Int clone setValue(n)) value)
		assertEquals(n, SInt ptr clone setValue(SInt clone setValue(n)) value)
	)

	testValueWithUnsignedInteger := method(
		appendProto(Types)
		
		n := 856
		assertEquals(n, UShort ptr clone setValue(UShort clone setValue(n)) value)
		assertEquals(n, UInt ptr clone setValue(UInt clone setValue(n)) value)
		assertEquals(n, ULong ptr clone setValue(ULong clone setValue(n)) value)
	)
	
	testValueWithDouble := method(
		appendProto(Types)
		
		double := -85.66747
		
		// see comment in testValueWithSignedInteger
		assertEquals(double, Double ptr clone setValue(Double clone setValue(double)) value)
	)
	
	testPointerToPointers := method(
		appendProto(Types)
		
		i    := Int clone setValue(-87)
		pi   := Int ptr clone setValue(i)
		ppi  := Int ptr ptr clone setValue(pi)
		pppi := Int ptr ptr ptr clone setValue(ppi)
		
		assertEquals("^i"  ,   pi typeString)
		assertEquals("^^i" ,  ppi typeString)
		assertEquals("^^^i", pppi typeString)
		
		assertEquals(i value,   pi value)
		assertEquals(i value,  ppi value value)
		assertEquals(i value, pppi value value value)
	)
	
	testAddress := method(
		appendProto(Types)
		
		pd   := Double ptr clone
		ppd  := Double ptr ptr clone
		pppd := Double ptr ptr ptr clone

		assertEquals(nil,   pd address)
		assertEquals(nil,  ppd address)
		assertEquals(nil, pppd address)
		
		pd   setValue(-76.894)
		ppd  setValue(pd)
		pppd setValue(ppd)

		assertEquals(pd  address,  ppd value address)
		assertEquals(pd  address, pppd value value address)
		assertEquals(ppd address, pppd value address)

	)

	testPointedToType := method(
		appendProto(Types)
		
		1pS := UShort ptr clone
		2pS := UShort ptr ptr clone
		3pS := UShort ptr ptr ptr clone
		4pS := UShort ptr ptr ptr ptr clone
		5pS := UShort ptr ptr ptr ptr ptr clone
		
		assertEquals(5pS proto, Types UShort ptr ptr ptr ptr ptr)
		assertEquals(5pS pointedToType,                                           4pS proto)
		assertEquals(5pS pointedToType pointedToType,                             3pS proto)
		assertEquals(5pS pointedToType pointedToType pointedToType,               2pS proto)
		assertEquals(5pS pointedToType pointedToType pointedToType pointedToType, 1pS proto)
		
		assertEquals(4pS proto, Types UShort ptr ptr ptr ptr)
		assertEquals(4pS pointedToType,                             3pS proto)
		assertEquals(4pS pointedToType pointedToType,               2pS proto)
		assertEquals(4pS pointedToType pointedToType pointedToType, 1pS proto)
		
		assertEquals(3pS proto, Types UShort ptr ptr ptr)
		assertEquals(3pS pointedToType,               2pS proto)
		assertEquals(3pS pointedToType pointedToType, 1pS proto)
		
		assertEquals(2pS proto, Types UShort ptr ptr)
		assertEquals(2pS pointedToType, 1pS proto)
		
		assertEquals(1pS proto, Types UShort ptr)
	)
	
	testCStringPointer := method(
		appendProto(Types)
		
		str1 := "abc"
		str2 := "def"
		
		cs   := CString clone setValue(str1)		
		csp1 := CString ptr clone setValue(cs)
		csp2 := CString ptr clone setValue(cs value)
		
		assertEquals(cs value, str1)
		assertEquals(cs value, csp1 value)
		assertEquals(cs value, csp2 value)
		
		cs setValue(str2)
		
		assertEquals(cs value, str2)
		assertEquals(cs value, csp1 value)
		assertEquals(str1, csp2 value)
	)
)
