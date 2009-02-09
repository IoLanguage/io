/*metadoc Rational Number description
Example use:
<pre>
r1 := Rational with(1, 2) /* 1/2 */
r1 + r1 == Rational with(1, 1) /* true */
Rational with(1, 1) == Rational with(4, 4) /* true */

r2 := Rational with(1, 4)
r1 + r2 == 0.75 asRational
</pre>
 */

Rational := Object clone do(
	//metadoc Rational category Math
	//metadoc Rational copyright Jeremy Tregunna, 2006
	//metadoc Rational license BSD

	//doc Rational numerator Returns the numerator.
	//doc Rational setNumerator(aNumber) Sets the numerator. Returns self.
	numerator ::= 0
	
	//doc Rational denominator Returns the denominator.
	//doc Rational setDenominator(aNumber) Sets the denominator. Returns self.	
	denominator ::= 0

	//doc Rational with(aNumerator, aDenominator) Convenience constructor. Returns a new Rational number whose numerator and denominator are represented by the arguments aNumerator and aDenominator respectively.")
	with := method(n, d,
		if(d < 0,
			n = -n
			d = -d
		)
		//writeln(n, ":", d)
		self clone setNumerator(n asNumber) setDenominator(d asNumber) reduce
	)

	//doc Rational +(aNum) Returns the value of the receiver added to aNum.
	+ := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator + b numerator * denominator, denominator * b denominator) reduce
		,
			asNumber + b
		)
	)

	//doc Rational -(aNum) Returns the value of the receiver subtracted from aNum.
	- := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator - b numerator * denominator, denominator * b denominator) reduce
		,
			asNumber - b
		)
	)

	//doc Rational *(aNum) Returns the value of the receiver multiplied by aNum.
	* := method(b,
		if(b isKindOf(Rational),
			with(numerator * b numerator, denominator * b denominator) reduce
		,
			asNumber * b
		)
	)

	//doc Rational /(aNum) Returns the value of the receiver divided by aNum.
	/ := method(b,
		if(b isKindOf(Rational),
			with(numerator * b denominator, denominator * b numerator) reduce
		,
			asNumber / b
		)
	)

	//doc Rational divmod(aNum) Returns a list containing the integer value and the receiver modulus aNum.
	divmod := method(b,
		v := (self / b) asNumber floor
		list(v, self - b * v)
	)

	//doc Rational %(aNum) Returns the receiver modulus aNum
	% := method(b, divmod(b) at(1))

	//doc Rational pow(aNum) Returns the value of the receiver to the power of aNum.
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

	//doc Rational abs Returns a Rational number with the absolute value of the receiver.
	abs := method(
		if(numerator > 0,
			self with(numerator, denominator)
		,
			self with(-numerator, denominator)
		)
	)

	//doc Rational negate Negates the Rational number.
	negate := method(Rational with(-numerator, denominator))

	//doc Rational ==(other) Compares two numbers against one another. Returns true if they are equal (represent the same number), false otherwise.
	== := method(b,
		if(b isKindOf(Rational),
			numerator == b numerator and denominator == b denominator
		,
			self asNumber == b
		)
	)

	//doc Rational !=(other) Compares two numbers against one another. Returns true if they are equal (represent the same number), false otherwise.
	!= := method(b,
		if(b isKindOf(Rational),
			numerator != b numerator or denominator != b denominator
		,
			self asNumber != b
		)
	)

	//doc Rational compare(other) Compares two numbers against one another. Returns a positive, zero or negative value based on whether the receiver is larger, equal or less than the argument.
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

	//doc Rational reduce Reduces the numerator and denominator to their lowest terms.
	reduce := method(
		gcd := numerator gcd(denominator)
		if(gcd < 0, gcd = - gcd)
		setNumerator(numerator / gcd)
		setDenominator(denominator / gcd)
		self
	)

	//doc Rational asNumber Converts the Rational number to a floating point number.
	asNumber := method(self numerator / self denominator)

	//doc Rational asString Returns a text string representing the Rational number.
	asString := method("#{numerator}/#{denominator}" interpolate)

	//doc Rational serialized Returns a bit of code which can be used to serialize the Rational number.
	serialized := method("Rational with(#{numerator}, #{denominator});" interpolate)
)

Number do(
	//doc Rational gcd(aNum) Calculates the greatest common denominator between the receiver and the argument.
	gcd := method(b,
		if(b == 0, return self)
		b gcd(self % b)
	)

	//doc Rational asRational Converts the number to a Rational number. CAVEAT: Numbers in Io are floating point entities, which means since they are imprecise, this conversion may yield values not expected.")
	asRational := method(Rational with(self, 1))
)
