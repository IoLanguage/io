Map do(
	docSlot("asList", "Converts a Map to a list of lists. Each element in the returned list will be a list of two elements: the key, and the value.")
	asList := method(
		l := list
		foreach(k, v, l append(list(k, v)))
		l
	)

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

	reverseMap := method(
		Map clone addKeysAndValues(values, keys)
	)
	
	asObject := method(
		o := Object clone
		self foreach(k, v, o setSlot(k, getSlot("v")))
		o
	)
)
