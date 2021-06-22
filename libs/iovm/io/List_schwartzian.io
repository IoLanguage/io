/*
http://www.quag.geek.nz/io/schwartzian/

A decorate-sort-undecorate operation for Io

Specify a key for each item, and control how the keys are compared.

Io> Range
Io> 0 to(10) asList sortByKey(x, (-1)**x * x)
==> list(9, 7, 5, 3, 1, 0, 2, 4, 6, 8, 10)

Io> Directory with(".") files sortKey(lastInfoChangeDate) sort map(name)
==> ... listing of directory sorted by date

Two forms of sortKey are supported:

	* files sortKey(lastInfoChangeDate)
	* files sortKey(file, file lastInfoChangeDate)

Three forms of sort are supported:

	* files sortKey(...) sort
	* files sortKey(...) sort(<)
	* files sortKey(...) sort(x, y, x < y)
*/

List sortByKey := method(
	call delegateToMethod(self, "sortKey") sort
)

List sortKey := method(
	schwart := call activated SchwartzianList clone
	if(call argCount == 1,
		body := call argAt(0)
		foreach(value,
			ss := stopStatus(k := value doMessage(body, call sender))
			if(ss isReturn, ss return(k))
			if(ss isBreak, break)
			if(ss isContinue, continue)
			schwart addPair(k, value)
		)
	,
		valueName := call argAt(0) name
		foreach(value,
			call sender setSlot(valueName, value)
			ss := stopStatus(k := call evalArgAt(1))
			if(ss isReturn, ss return(k))
			if(ss isBreak, break)
			if(ss isContinue, continue)
			schwart addPair(k, value)
		)
	)
	schwart
) do(
	SchwartzianList := Object clone do(
		newSlot("pairs")

		init := method(
			pairs = list
		)

		SchwartzianPair := Object clone do(
			newSlot("key")
			newSlot("value")

			asSimpleString := method(
				"(" .. key asSimpleString .. ": " .. value asSimpleString .. ")"
			)
		)

		addPair := method(key, value,
			pairs append(SchwartzianPair clone setKey(key) setValue(value))
		)

		sort := method(
			if(call argCount == 0,
				pairs sortBy(block(x, y, x key < y key))
			,
				if(call argCount == 1,
					opName := call argAt(0) name
					args := list(nil)
					pairs sortBy(
						block(x, y,
							x key performWithArgList(opName, args atPut(0, y key))
						)
					)
				,
					sortCall := call
					xName := sortCall argAt(0) name
					yName := sortCall argAt(1) name
					pairs sortBy(
						block(x, y,
							sortCall sender setSlot(xName, x key)
							sortCall sender setSlot(yName, y key)
							sortCall evalArgAt(2)
						)
					)
				)
			) mapInPlace(p, p value)
		)
	)
)
