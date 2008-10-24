Object do(
  //doc Object debugWriteln Method for writing debug messages (nil/writeln by default)
	debugWriteln := nil
	//doc Object debugOff Sets debugWriteln to nil. Returns self.
	debugOff := method(self debugWriteln := nil; self)
	//doc Object debugOn Sets debugWriteln to writeln. Returns self.
	debugOn := method(self debugWriteln := getSlot("writeln"); self)
	//doc Object ifDebug(code) Executes code if debugWriteln is not nil.
	ifDebug := method(
		if(debugWriteln, call evalArgAt(0))
	)
)