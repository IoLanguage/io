Number do(
	//doc Number isInASequenceSet Return true if receiver is in one of the Sequence sequenceSets, otherwise false.
	isInASequenceSet := method(
		Sequence sequenceSets foreach(set,
			if(in(set), return true)
		)
		false
	)

	//doc Number constants Object containing number constants e, inf, nan and pi.

	constants := Object clone do(
		//doc Number nan Returns a infinity constant.
		nan := 0/0

		//doc Number inf Returns a not-a-number constant.
		inf := 1/0

		//doc Number e Returns the constant e.
		e  := 2.71828182845904523536028747135266249

		//doc Number pi Returns the constant pi.
		pi := 3.14159265358979323846264338327950288
	)

	asSimpleString := method(self asString)

	//doc Number toBase(base) Returns the number in another base. 42 toBase(2) -> "101010"
	toBase := method(base, self asString toBase(base))

        //doc Number toBaseWholeBytes(base) Returns the number in another base printing out entire bytes. 42 toBaseWholeBytes(2) -> "00101010"
        toBaseWholeBytes := method(base,
            str := self asString toBase(base)
            byteColumns := 256 log(base) ceil
            alignedWidth := (str size / byteColumns) ceil * byteColumns
            str alignRight(alignedWidth, "0")
        )

	//doc Number asHex Returns the number as hex digits inside a string. 97 asHex -> "61"
	asHex := method(toBaseWholeBytes(16))

	//doc Number asBinary Returns the number as binary digits inside a string. 42 asBinary -> "101010"
	asBinary := method(toBaseWholeBytes(2))

	//doc Number asOctal Returns the number as octal digits inside a string. 436 asOctal -> "664"
	asOctal := method(toBaseWholeBytes(8))

	//doc Number combinations(size) Returns the combinations where the receiver is the number of different objects and size is the number to be arranged.
	combinations := method(r, self factorial /(r factorial *((self - r) factorial)))

	//doc Number permutations(size) Returns the permutations where the receiver is the number of different objects and size is the number to be arranged.
	permutations := method(r, self factorial /((self - r) factorial))

	//doc Number minMax(low, high) Returns a number between or equal to low and high. If the receiver is equal to or between low and high, the receiver is returned. If the receiver is less than low, low is returned. If the receiver is greater than high, high is returned.
	minMax := method(low, high, min(high) max(low))
	
	asJson := getSlot("asString")
)
