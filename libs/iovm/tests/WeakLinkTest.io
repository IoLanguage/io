
WeakLinkTest := UnitTest clone do(
	test1 := method(
		a := Object clone
		self w := WeakLink clone setLink(a)
		assertTrue(w link == a)
		//a = 1
		//Collector collect
	)
	test2 := method(
		Collector collect
		assertTrue(w link == nil)
	)
)
