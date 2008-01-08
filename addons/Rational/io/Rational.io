/* Rational Number
   Copyright (c) 2006 Jeremy Tregunna. All rights reserved.
 */

Rational := Object clone do(
	docCategory("Math")
	docObject("Rational")
	docCopyright("Jeremy Tregunna", 2006)
	docLicense("BSD")

	newSlot("numerator", 0)
	newSlot("denominator", 0)

	docSlot("with(aNumerator, aDenominator)", "Convenience constructor. Returns a new Rational number whose numerator and denominator are represented by the arguments aNumerator and aDenominator respectively.")
	with := method(n, d,
		if(d < 0,
			n = -n
			d = -d
		)
		//writeln(n, ":", d)
		self clone setNumerator(n asNumber) setDenominator(d asNumber) reduce
	)

	docSlot("+ aNum", "Returns the value of the receiver added to aNum.")
	+ := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator + b numerator * denominator, denominator * b denominator) reduce
		,
			asNumber + b
		)
	)

	docSlot("- aNum", "Returns the value of the receiver subtracted from aNum.")
	- := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator - b numerator * denominator, denominator * b denominator) reduce
		,
			asNumber - b
		)
	)

	docSlot("* aNum", "Returns the value of the receiver multiplied by aNum.")
	* := method(b,
		if(b isKindOf(Rational),
			with(numerator * b numerator, denominator * b denominator) reduce
		,
			asNumber * b
		)
	)

	docSlot("/ aNum", "Returns the value of the receiver divided by aNum.")
	/ := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator, denominator * b numerator) reduce
		,
			asNumber / b
		)
	)

	docSlot("divmod(aNum)", "Returns a list containing the integer value and the receiver modulus aNum.")
	divmod := method(b,
		v := (self / b) asNumber floor
		list(v, self - b * v)
	)

	docSlot("%(aNum)", "Returns the receiver modulus aNum")
	% := method(b, divmod(b) at(1))

	docSlot("pow(aNum)", "Returns the value of the receiver to the power of aNum.")
	pow := method(b,
		if(b isKindOf(Rational), b = b asNumber)
		r := self clone
		if(b != 0,
			r setNumerator(numerator pow(if(b > 0, b, -b)))
			r setDenominator(denominator pow(if(b > 0, b, -b)))
		,
			r setNumerator(1)
			r setDenominator(1)
		)
	)

	docSlot("abs", "Returns a Rational number with the absolute value of the receiver.")
	abs := method(
		if(numerator > 0,
			self with(numerator, denominator)
		,
			self with(-numerator, denominator)
		)
	)

	docSlot("negate", "Negates the Rational number.")
	negate := method(Rational with(-numerator, denominator))

	docSlot("== other", "Compares two numbers against one another. Returns true if they are equal (represent the same number), false otherwise.")
	== := method(b,
		if(b isKindOf(Rational),
			numerator == b numerator and denominator == b denominator
		,
			self asNumber == b
		)
	)

	docSlot("!= other", "Compares two numbers against one another. Returns true if they are equal (represent the same number), false otherwise.")
	!= := method(b,
		if(b isKindOf(Rational),
			numerator != b numerator or denominator != b denominator
		,
			self asNumber != b
		)
	)

	docSlot("compare(other)", "Compares two numbers against one another. Returns a positive, zero or negative value based on whether the receiver is larger, equal or less than the argument.")
	compare := method(b,
		if(b isKindOf(Rational),
			r := numerator * b denominator - b numerator * denominator
			if(r == 0,
				return 0
			,
				return if(r < 0, -1, 1)
			)
		,
			asNumber compare(b)
		)
	)

	docSlot("reduce", "Reduces the numerator and denominator to their lowest terms.")
	reduce := method(
		gcd := numerator gcd(denominator)
		if(gcd < 0, gcd = - gcd)
		setNumerator(numerator / gcd)
		setDenominator(denominator / gcd)
		self
	)

	docSlot("asNumber", "Converts the Rational number to a floating point number.")
	asNumber := method(self numerator / self denominator)

	docSlot("asString", "Returns a text string representing the Rational number.")
	asString := method("#{numerator}/#{denominator}" interpolate)

	docSlot("serialized", "Returns a bit of code which can be used to serialize the Rational number.")
	serialized := method("Rational with(#{numerator}, #{denominator});" interpolate)
)

Number do(
	docSlot("gcd(aNum)", "Calculates the greatest common denominator between the receiver and the argument.")
	gcd := method(b,
		if(b == 0, return self)
		b gcd(self % b)
	)

	docSlot("asRational", "Converts the number to a Rational number. CAVEAT: Numbers in Io are floating point entities, which means since they are imprecise, this conversion may yield values not expected.")
	asRational := method(Rational with(self, 1))
)
