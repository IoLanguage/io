Range do(
	//doc Range asList Returns a list containing all the items within and including the ranges starting and ending points.
	asList := method(
		lst := List clone
		self foreach(v, lst append(v))
		lst
	)

	//doc Range contains Returns a boolean value if the range contains the argument.
	contains := method(value,
		lst := self clone
		start := lst first
		# Calculating the step. Use 'nil' if range contains only one element.
		step := if(lst next isNil, nil, lst value - start)
		end := lst last
		# return true if value between start and end if 
		# difference between value and end dividing by step without a remainder
		return (value between(start, end) and if(step isNil, value == start, (value - start)%step) == 0)
	)

	//doc Range select Operates the same as 'List select'
	select := List getSlot("select")

	/*doc Range map(optionalIndex, value, message)
	Returns a new list which contains the result of the 'body' for every element 
	stepped over in the range, from the starting point to the ending point inclusive. 
	This method can operate several ways; these include: 
	(1) Takes one argument, the message tree to be executed during each iteration; (2) 
	Takes two arguments, the first argument is the name of the current value being 
	iterated over, and the second is the message tree to be executed during each 
	iteration; (3) Takes three arguments: the first is the current index within the 
	range, the second is the name of the current value being iterated over, and the 
	third is the message tree to be executed during each iteration. 
	For example:
<pre>
# First method (operating on numbers)
1 to(10) map(*2) # multiply each value by two
# Second method (operating on numbers)
1 to(10) map(v, "#{v}" interpolate) # returns string representation of each value as list
# Third method (operating on numbers)
1 to(10) map(i, v, i*v) #  multiply each value by index
</pre>
	 */
	map := method(
		call delegateToMethod(self asList, "mapInPlace")
	)

	//doc Range indexOf(aValue) Calculates each value, checking to see if it matches the aValue parameter. If so, return the position within the range. NOTE: This method rewinds the range before searching. If you need to revert back to your original position, make a duplicate of the range, and use indexOf on it instead.
	indexOf := method(aValue,
		rewind
		while(value != aValue,
			self = next ifNilEval(return nil)
		)
		index
	)

	//doc Range at(position) Rewinds the range, skips forward until we're at the supplied position then returns the value at that position. Raises an exception if the position is out of bounds.
	at := method(pos,
		rewind
		while(index != pos,
			self = next ifNilEval(Exception raise("Index out of bounds"))
		)
		value
	)

	//doc Range slice(start, end, [by]) Returns a list containing the values from the Range starting at the start parameter, ending at the end parameter, and optionally incremented by the by parameter.
	slice := method(s, e, b,
		if(e compare(s) < 0, Exception raise("Starting point must be greater than the ending point."))
		// keep slice without side-effects
		self = self clone
		if(b isNil, b = 1)
		l := list
		s repeat(self = self next)
		i := s
		while(i <= e,
			l append(value)
			self = b repeat(self = self next)
			i = i + b
		)
		l
	)
)


Number do(
	//doc Range to Convenience constructor that returns a cursor object representing the range of numbers from the receiver to the 'endingPoint' parameter. Increments over each item in that range by 1.
	to := method(e, self toBy(e, 1))

	//doc Range toBy(endingPoint, incrementValue) Convenience constructor that returns a cursor object representing the range of numbers from the receiver to the 'endingPoint' parameter. Increments over each item in that range by the 'incrementValue' parameter.
	toBy := method(e, i,
		if(i < 0, Exception raise("increment value must be non-negative"))
		if((e - self) < i, e = self)
		if(self > e, i = 0 - i)
		Range clone setRange(self, e, i)
	)
)
