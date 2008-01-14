Object do(
	error := nil
	shouldRaiseOnError := false
	
	debugWriteln := nil
	debugOff := method(self debugWriteln := nil; self)
	debugOn := method(self debugWriteln := getSlot("writeln"); self)
	ifDebug := method(
		if(debugWriteln, call evalArgAt(0))
	)
	
	raiseOnError := method(
		if(call message arguments size == 1,
			oldShouldRaiseOnError := shouldRaiseOnError
			shouldRaiseOnError = true
			e := try(result := call evalArgAt(0))
			shouldRaiseOnError = oldShouldRaiseOnError
			if(e, e pass)
			result
		,
			shouldRaiseOnError = true
			self
		)
	)
	
	dontRaiseOnError := method(
		if(call message arguments size == 1,
			oldShouldRaiseOnError := shouldRaiseOnError
			shouldRaiseOnError = false
			e := try(result := call evalArgAt(0))
			shouldRaiseOnError = oldShouldRaiseOnError
			if(e, e pass)
			result
		,
			shouldRaiseOnError = false
			self
		)
	)
	
	setError := method(errorMessage,
		if(shouldRaiseOnError,
			Exception raise(errorMessage)
		,
			error = errorMessage
		)
	)
)