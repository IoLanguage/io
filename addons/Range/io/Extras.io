Number do(
		//doc Range nextInSequence(skipVal) Returns the next item in the sequence counting away from zero. The optional skipVal parameter allows you to skip the number specified and get that value (the next value after self + skipVal).
	nextInSequence := method(skipVal,
		if(skipVal isNil, skipVal = 1)
		self + skipVal
	)

		//doc Range to Convenience constructor that returns a cursor object representing the range of numbers from the receiver to the 'endingPoint' parameter. Increments over each item in that range by 1.
	to := method(e,i,
		i ifNil(i = if(self < e, 1, -1))
		Range clone setRange( self, if( (e - self) abs < i abs, self, e ), i )
	)

		//doc Range toBy(endingPoint, incrementValue) Convenience constructor that returns a cursor object representing the range of numbers from the receiver to the 'endingPoint' parameter. Increments over each item in that range by the 'incrementValue' parameter.
	toBy := getSlot("to")
)


Sequence do(
		//doc Range nextInSequence(skipVal) Returns the next item in the sequence.  The optional skipVal parameter allows you to skip ahead skipVal places.
	nextInSequence := method(skipVal,
		str := self clone asMutable
		skipVal ifNil(skipVal = 1)

		if(size < 1, return str asSymbol)

		leadingNonNextableChars := 0
		str foreach(char,
			done := false
			countIt := true
			sequenceSets foreach(set,
				set indexOf(char) ifNonNil(countIt = false; done = true)
			)
			if(countIt, leadingNonNextableChars = leadingNonNextableChars + 1)
			if(done, break)
		)
		(leadingNonNextableChars == str size) ifTrue(return str asSymbol)

		str := str splitAt(leadingNonNextableChars)
		leadingNonNextables := str at(0)
		str := str at(1) asMutable

		0 to(str size - 1) asList reverseForeach(index,
			done := false
			sequenceSets foreach(setName, set,
				x := set indexOf(str at(index))
				if(x,
					x = x + 1
				,
					continue
				)
				if(x == set size,
					str atPut(index, set at(0))
					if(index == 0,
						str prependSeq(set at(if(setName == "digitSequence", 1, 0)) asCharacter)
						done = true
					)
				,
					str atPut(index, set at(x))
					done = true
				)
				break
			)
			if(done, break)
		)

		str = leadingNonNextables .. str
		skipVal = skipVal - 1
		if(skipVal < 1,
			str asSymbol
		,
			str nextInSequence(skipVal)
		)
	)

		//doc Range to(endpoint) Convenience constructor that returns a range of sequences from the receiver to the endpoint argument. Increments over each item in that range by 1.
	to := method(e, toBy(e, 1))

		//doc Range toBy(endpoint, increment) Convenience constructor that returns a range of sequences from the receiver to the endpoint argument. Increments over each item in that range by the value of the increment parameter. The increment parameter must be positive.
	toBy := method(e, i,
		toByCompare := method(a, b,
			(a size == b size) ifTrue(return a compare(b))
			nulls := "" asMutable
			(a size - b size) abs repeat(nulls appendSeq("\0"))
			if(a size < b size,
				a = nulls .. a
			,
				b = nulls .. b
			)
			a compare(b)
		)
		removeLeadingZerosAfterLeadingSymbols := method(str,
			if(str isMutable not, str = str asMutable)

			# skip over leading symbols
			from := 0
			str foreach(char,
				if(char isInASequenceSet, break, from = from + 1)
			)
			# slice off leading '0's
			to := from - 1
			str slice(from) foreach(char, if(char == "0" at(0), to = to + 1, break))
			if(from <= to, str removeSlice(from, to), str)
		)


		from := removeLeadingZerosAfterLeadingSymbols(self asMutable strip)
		to := removeLeadingZerosAfterLeadingSymbols(e asMutable strip)

		#if(i < 0, Exception raise("increment value must be non-negative"))
		(i == 1) ifFalse(Exception raise("increment value must be 1"))
		if(toByCompare(from, to) > 0, Exception raise("range must increase"))

		symbols := list
		1 to(from size min(to size)) foreach(index,
			index = 0 - index
			(from at(index) isInASequenceSet == to at(index) isInASequenceSet) ifFalse(
				Exception raise("invalid range")
			)
		)

		if(from size != to size,
			smaller := bigger := nil
			if(from size > to size,
				bigger = from; smaller = to
			,
				bigger = to; smaller = from
			)
			growthChars := bigger slice(0, bigger size - (smaller size))
			setOfFirstCharInSmaller := nil
			sequenceSets foreach(set,
				if(smaller at(0) in(set), setOfFirstCharInSmaller := set; break)
			)
			growthChars foreach(v,
				v in(setOfFirstCharInSmaller) ifFalse(
					Exception raise("invalid range")
				)
			)
		)

		Range clone setRange(from, to, i)
	)

		//doc Range levenshtein(other) Returns the levenshtein distance to other.
	levenshtein := method(other,
		if(other size < self size, return other levenshtein(self))
		current := 0 to(self size) asList
		other foreach(i, otheri,
			previous := current
			current = List clone with(i + 1)
			self foreach(j, selfj,
				current append((current at(j) + 1) min(previous at(j + 1) + 1) min(previous at(j) + if(selfj == otheri, 0, 1)))
			)
		)
		current last
	)
)
