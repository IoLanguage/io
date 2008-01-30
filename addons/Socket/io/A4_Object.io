Object do(
	debugWriteln := nil
	debugOff := method(self debugWriteln := nil; self)
	debugOn := method(self debugWriteln := getSlot("writeln"); self)
	ifDebug := method(
		if(debugWriteln, call evalArgAt(0))
	)
)