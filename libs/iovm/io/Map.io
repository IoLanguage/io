Map do(
	//doc Map with(key1, value1, key2, value2, ...) Returns a new map containing the given keys and values
	with := method(
		m := Map clone
		args := call message arguments
		for(i, 0, args size - 1, 2,
			m atPut(call evalArgAt(i), call evalArgAt(i+1))
		)
		m
	)

	//doc Map asJson Converts a Map to a string that represents contents in JSON-compilant form
	asJson := method(
		"{" .. self keys map(k, k asJson .. ":" .. self at(k) asJson) join(",") .. "}"
	)

	//doc Map asList Converts a Map to a list of lists. Each element in the returned list will be a list of two elements: the key, and the value.
	asList := method(
		self keys map(k, list(k, self at(k)))
	)

	/*doc Map map(key, value, message)
		Create a List of results of message applied to self's items in a random
		order.
	*/
	map := method(
		result := List clone
		key := call argAt(0)
		value := call argAt(1)
		body := call argAt(2)
		self foreach(k, v,
			call sender setSlot(key name, k)
			call sender setSlot(value name, v)
			ss := stopStatus(r := call sender doMessage(body))
			if(ss isReturn, call setStopStatus(ss); return getSlot("v"))
			if(ss isBreak, break)
			if(ss isContinue, continue)
			result append(getSlot("r"))
		)
		result
	)

	/*doc Map select(optionalIndex, optionalValue, message)
		Creates a new Map with keys,values of self for which message evaluated
		to non-nil.
	*/
	select := method(
		result := Map clone
		self keys foreach(key,
			if(call argCount > 1,
				call sender setSlot(call argAt(0) name, key)
				if(call argCount == 3,
					call sender setSlot(call argAt(1) name, self at(key))
				)
			)
			ss := stopStatus(v := call evalArgAt(call argCount - 1))
			if(ss isReturn, call setStopStatus(ss); return getSlot("v"))
			if(ss isBreak, break)
			if(ss isContinue, continue)
			if(getSlot("v"),
				result atPut(key, self at(key))
			)
		)
		result
	)

	/*doc Map detect(optionalIndex, optionalValue, message)
		Returns a random value for which message evals to non-nil.
	*/
	detect := method(
		self keys foreach(key,
			if(call argCount > 1,
				call sender setSlot(call argAt(0) name, key)
				if(call argCount == 3,
					call sender setSlot(call argAt(1) name, self at(key))
				)
			)
			ss := stopStatus(v := call evalArgAt(call argCount - 1))
			if(ss isReturn, call setStopStatus(ss); return getSlot("v"))
			if(ss isBreak, break)
			if(ss isContinue, continue)
			if(getSlot("v"),
				return list(key, self at(key))
			)
		)
	)

	/*doc Map merge(anotherMap)
		Returns a new map created by merging the pairs from anotherMap into this map
	*/
	merge := method(anotherMap,
		self clone mergeInPlace(anotherMap)
	)

	/*doc Map mergeInPlace(anotherMap)
		Merges the pairs from anotherMap into this map
	*/
	mergeInPlace := method(anotherMap,
		addKeysAndValues(anotherMap keys, anotherMap values)
	)

	//doc Map reverseMap Creates a new Map using values as keys and keys as values
	reverseMap := method(
		Map clone addKeysAndValues(values, keys)
	)

	//doc Map asObject Create a new Object whose slotDescriptionMap will be equal to self
	asObject := method(
		o := Object clone
		self foreach(k, v, o setSlot(k, getSlot("v")))
		o
	)

	//doc Map isEmpty Returns true if this map doesn't contain any pairs.
	isEmpty := method(
		keys isEmpty
	)

    //doc Map isNotEmpty Returns true if this map contains at least one pair.
    isNotEmpty := method(
        keys isNotEmpty
    )
)
