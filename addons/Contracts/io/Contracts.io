/* Design By Contract - A Minor Language Feature
   Copyright (c) 2006 Jeremy Tregunna. All rights reserved.
 */

/* The following is only needed to facility bundle loading */
Contracts := Object clone

Object do(
	contract := method(
		args := call message arguments
		body := args removeLast
		argNames := args map(name)
		pre := true
		selfBlock := getSlot("self") getSlot("Block")
		testConditionMessage := method("""if(#{call evalArgAt(0)}, self, Exception raise("#{call argAt(0) name}-condition failed"))""" interpolate asMessage)
		if(call message attached ?name == "setPreCondition",
			pre = call message attached argAt(0)
			call message setAttached(call message attached attached)
			if(call message attached ?name != "setPostCondition",
				body = testConditionMessage(pre) setNext(body)
			)
		)
		if(call message attached ?name == "setPostCondition",
			post := call message attached argAt(0)
			call message setAttached(call message attached attached)
			m := """
				__body := "#{body}"
				__args := call evalArgs
				__meth := #{selfBlock} clone setMessage(__body asMessage) setArgumentNames(#{argNames})
				__preMethod := #{selfBlock} clone setMessage(message(#{testConditionMessage(pre)})) setArgumentNames(#{argNames}) setIsActivatable(true)
				__tempMsg := message(__preMethod) setArguments(call message arguments)
				getSlot("__preMethod") performOn(call sender, Object clone, __tempMsg)
				__tempMsg = message(__meth) setArguments(call message arguments)
				result := getSlot("__meth") performOn(call sender, Object clone, __tempMsg)
				if(#{testConditionMessage(post)} == self, result)
			""" interpolate asMessage
			selfBlock clone setMessage(m) setArgumentNames(argNames) setIsActivatable(true)
		,
			selfBlock clone setMessage(body) setArgumentNames(argNames) setIsActivatable(true)
		)
	)
)
