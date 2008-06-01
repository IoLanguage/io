
ObsidianTest := UnitTest clone do(	
	testBasic := method(
		/*
		ObsidianServer clone @start
		yield
		
		client := ObsidianClient clone open
		client onAtPut("1", "aKey", "aSlot")
		client onAtPut("1", "bKey", "bSlot")
		client onAtPut("1", "cKey", "cSlot")
		
		assertEquals(client first("1", 5), list("aKey", "bKey", "cKey"))

		assertEquals(client onAt("1", "aKey"), "aSlot")
		assertEquals(client onAt("1", "bKey"), "bSlot")
		assertEquals(client onAt("1", "cKey"), "cSlot")

		client close
		ObsidianServer stop
		*/
	)
) 