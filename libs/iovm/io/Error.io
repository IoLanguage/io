Object do(
	ifError := method(self)
	returnIfError := getSlot("ifError")
	raiseIfError  := getSlot("ifError")
	isError := false
)

Error := Object clone do(
	ifError := method(
		if(call message arguments size == 1) then(
			return(call evalArgAt(0))
		) elseif(call message arguments size > 1) then(
			call sender setSlot(call message argAt(0) name, self)
			return(call evalArgAt(1))
		) else(
			Exception raise("ifError requires at least 1 argument")
		)
	) setPassStops(true)
	
	returnIfError := method(
		call sender return(self)
	) setPassStops(true)
	
	raiseIfError := method( //message,
		Exception raise(message)
	)
	
	with := method(message,
		self message := message
		//Coroutine currentCoroutine setErrorDescription(message);
	)
	
	isError := true
)