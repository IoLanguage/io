
/*
Sequence asChars := method(
	b := Sequence clone
	self foreach(i, v, b write(v asString, " "))
	b
)
*/
		
BlowfishTest := UnitTest clone do(
	testBasic := method(
		s := "This is a test."
			
		es := Blowfish clone setKey("secret") encrypt(s)
		ds := Blowfish clone setKey("secret") decrypt(es)
		ds setSize(s size)
		
		assertEquals(ds, s)
	)
) 