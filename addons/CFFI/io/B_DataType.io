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
	)


	Types := Object clone do(
		SChar := Char := DataType clone setTypeString("c")
		UChar := DataType clone setTypeString("C")

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
