ContinuedFraction := Object clone do(
	//metadoc ContinuedFraction category Math
	//metadoc ContinuedFraction description ?
	init := method(
		self list := List clone
		self precisionx := 0
		self precisiony := 0
		self coefficients := List with(0, 0, 0, 0, 1, 0, 0, 0)
		self
	)
	with := method(n,
		result := ContinuedFraction clone
		result coefficients atPut(0, n)
		result
	)
	+ := method(other,
		result := ContinuedFraction clone
		result x := self
		result y := other
		result coefficients := List with(0, 1, 1, 0, 1, 0, 0, 0)
		result
	)
	- := method(other,
		result := ContinuedFraction clone
		result x := self
		result y := other
		result coefficients := List with(0, 1, -1, 0, 1, 0, 0, 0)
		result
	)
	* := method(other,
		result := ContinuedFraction clone
		result x := self
		result y := other
		result coefficients := List with(0, 0, 0, 1, 1, 0, 0, 0)
		result
	)
	/ := method(other,
		result := ContinuedFraction clone
		result x := self
		result y := other
		result coefficients := List with(0, 1, 0, 0, 0, 0, 1, 0)
		result
	)
	at := method(index,
		while(list size <= index,
			res := 1/0
			while(true,
				if(coefficients at(4) == 0 and coefficients at(5) == 0 and coefficients at(6) == 0 and coefficients at(7) == 0, break)
				i11 := if(coefficients at(0) == 1/0 or coefficients at(4) == 0, res, (coefficients at(0) / coefficients at(4)) round)
				i10 := if(coefficients at(1) == 1/0 or coefficients at(5) == 0, if(coefficients at(1) == 0, i11, res), (coefficients at(1) / coefficients at(5)) round)
				i01 := if(coefficients at(2) == 1/0 or coefficients at(6) == 0, if(coefficients at(2) == 0, i10, res), (coefficients at(2) / coefficients at(6)) round)
				i00 := if(coefficients at(3) == 1/0 or coefficients at(7) == 0, if(coefficients at(3) == 0, i01, res), (coefficients at(3) / coefficients at(7)) round)
				if(i11 == i10 and i10 == i01 and i01 == i00) then(
					res = i11
					if (res == 1/0, break)
					coefficients = List with(
						coefficients at(4),
						coefficients at(5),
						coefficients at(6),
						coefficients at(7),
						if (res == 1/0, coefficients at(4), coefficients at(0) - coefficients at(4) * res),
						if (res == 1/0, coefficients at(5), coefficients at(1) - coefficients at(5) * res),
						if (res == 1/0, coefficients at(6), coefficients at(2) - coefficients at(6) * res),
						if (res == 1/0, coefficients at(7), coefficients at(3) - coefficients at(7) * res)
					)
					break
				) else(
					p := x at(precisionx)
					precisionx = precisionx + 1
					coefficients = List with(
						coefficients at(1),
						coefficients at(3),
						if(p == 1/0, coefficients at(1), coefficients at(0) + coefficients at(1) * p),
						if(p == 1/0, coefficients at(3), coefficients at(2) + coefficients at(3) * p),
						coefficients at(5),
						coefficients at(7),
						if(p == 1/0, coefficients at(5), coefficients at(4) + coefficients at(5) * p),
						if(p == 1/0, coefficients at(7), coefficients at(6) + coefficients at(7) * p)
					)
					tmp := x
					x = y
					y = tmp
					tmp = precisionx
					precisionx = precisiony
					precisiony = tmp
				)
			)
			list append(res)
		)
		list at(index)
	)
)

ContinuedFraction SQRT := ContinuedFraction clone do(
	with := method(n,
		result := SQRT clone
		result n := n
		result r := n sqrt
		result v := 1
		result a := result r floor
		result u := result a
		result
	)
	at := method(index,
		while(list size <= index,
			list append(a)
			if(a == 1/0, continue)
			v = (n - u ** 2) / v
			a = ((r + u) / v) floor
			u = a * v - u
		)
		list at(index)
	)
)

ContinuedFraction E := ContinuedFraction clone do(
	at := method(index,
		if (index == 0, return 2)
		if (index % 3 == 2, return 2*(index+1)/3)
		1
	)
)
