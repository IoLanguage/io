Object do(
  //doc Object ifError Does nothing, returns self.
	ifError := method(self)
	//doc Object returnIfError Does nothing, returns self.
	returnIfError := getSlot("ifError")
	//doc Object raiseIfError Does nothing, returns self.
	raiseIfError  := getSlot("ifError")
	//doc Object isError Returns false if not an error.
	isError := false
)

Error do(
  /*doc Error ifError(code) 
  Evaluates code in the context of sender. Returns self.
  <br/>
  Another form is <tt>Error ifError(error, code)</tt>. 
  Note: <tt>error</tt> slot is set in the context of sender, Locals object is not created!
  */
	ifError := method(
		if(call message arguments size == 1) then(
			call evalArgAt(0)
		) elseif(call message arguments size > 1) then(
			call sender setSlot(call message argAt(0) name, self)
			call evalArgAt(1)
		) else(
			Exception raise("ifError requires at least 1 argument")
		)
		self
	) setPassStops(true)
	
	//doc Error returnIfError Returns self from the context of sender.
	returnIfError := method(
		call sender return(self)
	) setPassStops(true)
	
	//doc Error raiseIfError Raises exception.
	raiseIfError := method( //message,
		Exception raise(message)
	)
	
	//doc Error with(message) Returns new error with message slot set.
	with := method(message,
		error := self clone
		error message := message
		error location := call message label .. ":" .. call message lineNumber
		error
	)

	withShow := method(s,
		writeln("ERROR: ", s)
		error := self clone
		error message := message
		error location := call message label .. ":" .. call message lineNumber
		error
	)
		
	//doc Error isError Returns true.
	isError := true
)
