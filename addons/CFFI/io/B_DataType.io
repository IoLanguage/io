/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/

CFFI do(
	DataType do(
		newSlot("typeString")

		init := method(
			self ptr := method(
				Pointer toType(self)
			)

			self init := method(
				self ptr := method(
					self proto ptr clone setValue(self)
				)
			)
		)

		with := method(value,
			this := self clone
			this setValue(value)
			this
		)
	)

	/*Object _updSlot := Object getSlot("updateSlot")
	Object updateSlot := method(a,b,
		obj := self getSlot(a)
		if(obj isKindOf(Structure) then(obj performWithArgList("setValues", b))
		elseif(obj isKindOf(DataType) then(obj setValue(b)
		else(self _updSlot(a, b))
		self
	)*/

	Types := Object clone do(
		SChar := Char := DataType clone setTypeString("c")
		UChar := DataType clone setTypeString("C")

		SByte := Byte := DataType clone setTypeString("b")
		UByte := DataType clone setTypeString("B")

		SShort := Short := DataType clone setTypeString("s")
		UShort := DataType clone setTypeString("S")

		SInt := Int := DataType clone setTypeString("i")
		UInt := DataType clone setTypeString("I")

		SLong := Long := DataType clone setTypeString("l")
		ULong := DataType clone setTypeString("L")

		Float := DataType clone setTypeString("f")

		Double := DataType clone setTypeString("d")

		Void := DataType clone setTypeString("v")

		CString := DataType clone setTypeString("*")
		/*CString castTo := method(type,
			if(type isKindOf(Pointer),
				return type clone setValue(self) value)
			nil
		)*/

		types := method(
			m := Map clone
			self slotNames foreach(name,
				slot := self getSlot(name)
				slot hasProto(DataType) ifTrue(m atPut(name asLowercase, slot))
			)
			m
		) call

		at := method(name,
			name = name asMutable strip
			if(name beginsWithSeq("unsigned "), name = name slice("unsigned " size) prependSeq("u"))
			if(name beginsWithSeq("signed "),   name = name slice("signed " size)   prependSeq("s"))
			if(name endsWithSeq("*"),
				name removeLast strip
				if(name endsWithSeq("*"),
					Pointer toType(Types at(name))
				,
					Pointer toType(types at(name))
				)
			,
				types at(name)
			)
		)
	)
	Types removeSlot("type") // we are a namespace
)
