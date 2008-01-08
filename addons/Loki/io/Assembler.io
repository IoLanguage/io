/*
	-- Loki -- x86 low level assembly toolbox --
*/

true cond := true
false cond := method( if (call evalArgAt(0), call evalArgAt(1); return true;); false)

List asNakedString := method(c,if (self isEmpty,"",self slice(0,-1) map(asString .. c) append(self last asString) reduce(..)))

setSlotWithType("x86",Object clone)
x86 do(

	/*
		-- Instruction database --
	*/

	// Database created from the NASM documentation
	doFile("x86opcodes.io")

	// fix Jcc NEAR
	x86opcodes at("Jcc") atPut(0, list("imm", "", "0F80+ccrw/rd", 1))
//	x86opcodes at("SETcc") push(list("r/m32", "", "0F90+cc/2", 1)) // fake 32bit version


	x86opcodes atPut("CMP",	list(
		list("r/m8,reg8", "", "38/r", 0),
		list("r/m16,reg16", "o16", "39/r", 0),
		list("r/m32,reg32", "o32", "39/r", 1),
		list("reg8,r/m8", "", "3A/r", 0),
		list("reg16,r/m16", "o16", "3B/r", 0),
		list("reg32,r/m32", "o32", "3B/r", 1),
		list("r/m8,imm8", "", "80/7ib", 0),
		list("r/m16,imm16", "o16", "81/7iw", 0),
		list("r/m32,imm32", "o32", "81/7id", 1),
		list("r/m16,imm8", "o16", "83/7ib", 0),
		list("r/m32,imm8", "o32", "83/7ib", 1),
		list("AL,imm8", "", "3Cib", 0),
		list("AX,imm16", "o16", "3Diw", 0),
		list("EAX,imm32", "o32", "3Did", 1)
	))

	conditionCodes := list(
		list(0,"O"),				//	(trigger if the overflow flag is set)
		list(1,"NO"),
		list(2,"B","C","NAE"),	// (trigger if the carry flag is set)
		list(3,"AE","NB","NC"),
		list(4,"E","Z"),			// (trigger if the zero flag is set)
		list(5,"NE","NZ"),
		list(6,"BE","NA"),		// (trigger if either of the carry or zero flags is set)
		list(7,"A","NBE"),
		list(8,"S"),				// (trigger if the sign flag is set)
		list(9,"NS"),
		list(10,"P","PE"),			//	(trigger if the parity flag is set)
		list(11,"NP","PO"),
		list(12,"L","NGE"),			// (trigger if exactly one of the sign and overflow flags is set)
		list(13,"GE","NL"),
		list(14,"LE","NG"),
		// (trigger if either the zero flag is set, or exactly one of the sign and overflow flags is set)
		list(15,"G","NLE")
	)

	ccOps := list("CMOV", "J", "SET")
	getConditionCode := method( name,
		op := ccOps select(s, name beginsWithSeq(s) )
		if ( op size == 0, return nil )
		tail := name slice(op at(0) size)
		conditionCodes foreach(l,
			match := l slice(1) select(== tail)
			if( match size > 0, return list(l at(0), match at(0) ) )
		)
		nil
	)

	ccFlat := conditionCodes flatten select( isKindOf(Sequence) )

	// x86opcodes keys select( endsWithSeq("cc") ) map( slice(0,-2) )


	/*
		-- Operands --
	*/

	Operand := Object clone do(
		appendProto(x86)
		print := method( asString print )
		asOperand := method(self)
	)

	Label := Operand clone do(
		newSlot("target")
		asString := method( target asString )

		matchSyntax := method(str,
			c := str endsWithSeq("imm")
		)
	)

//	label := method(n, Label clone setName(n))


	Immediate := Operand clone do(
		newSlot("value")
	//	value

		with := method(v, self clone setValue(v) )

		matchSyntax := method(str,
			c := str beginsWithSeq("imm")
			c
		)

	//	asString := method( "$#{value}" interpolate)
		asString := method(
			"$#{value}" interpolate
		)

		asStringCompact := method(
			if (value>0,
				"+#{value}" interpolate,
				if (value==0,"","#{value}" interpolate)
			)
		)
	)

	Number asOperand := method( x86 Immediate with(self) )

	MemoryReference := Operand clone do(

			newSlot("bits")

			base := nil
			index := nil
			scale := nil
			disp := nil
//			scale := 1
//			disp := 0
//			scale := 1
//			disp := 0

			matchSyntax := method(str,

			c := false
			(bits isNil or bits == 8) ifTrue(	c := c or str endsWithSeq("mem8") )
			(bits isNil or bits == 16) ifTrue(	c := c or str endsWithSeq("mem16") )
			(bits isNil or bits == 32) ifTrue(	c := c or str endsWithSeq("mem32") )
			(bits isNil or bits == 64) ifTrue(	c := c or str endsWithSeq("mem64") )
			(bits isNil or bits == 80) ifTrue(	c := c or str endsWithSeq("mem80") )
			c := c or str endsWithSeq("mem")

			(bits isNil or bits == 8) ifTrue(	c := c or str endsWithSeq("/m8") )
			(bits isNil or bits == 16) ifTrue(	c := c or str endsWithSeq("/m16") )
			(bits isNil or bits == 32) ifTrue(	c := c or str endsWithSeq("/m32") )
			(bits isNil or bits == 64) ifTrue(	c := c or str endsWithSeq("/m64") )
			c
		)

		asStringSemantic := method(
			baseSeq := "0"
			indexSeq := "0"
			if(base isKindOf(Register), baseSeq = base asString )
			if(index isKindOf(Register), indexSeq = base asString )
			if(bits,
				"ref: #{baseSeq}+#{scale}*#{indexSeq}+(#{disp}) [#{bits} bits]" interpolate,
				"ref: #{baseSeq}+#{scale}*#{indexSeq}+(#{disp}) [default size]" interpolate
			)
		)
		asStringSyntax := method(

		//	if (bits isNil not, return "[#{bits} bits]" interpolate)

			if(base,
				if (index,
					"[#{base}#{disp asStringCompact}+#{index}*#{scale}]" interpolate,
					"[#{base}#{disp asStringCompact}]" interpolate
				),
				asStringSemantic
			//	"MEMORY REFERENCE"
			)

		)

		asString := getSlot("asStringSyntax")
	//	asString := getSlot("asStringSemantic")
	)

	Register := Operand clone do(
		newSlot("id")
		newSlot("name")

	//	asString := method( "#{self type}.#{id}" interpolate)
		asString := method( "#{self name}" interpolate)

	)

	setSlotWithType("reg8", Register clone)
	setSlotWithType("reg16", Register clone)
	setSlotWithType("reg32", Register clone)
	setSlotWithType("fpureg", Register clone)
	setSlotWithType("mmreg", Register clone)
	setSlotWithType("xmmreg", Register clone)


	reg8 do(
		matchSyntax := method(str,
			c := str beginsWithSeq("reg8")
			c := c or (str beginsWithSeq("r/m8"))
			c
		)
	)

	reg16 do(
		matchSyntax := method(str,
			c := str beginsWithSeq("reg16")
			c := c or (str beginsWithSeq("r/m16"))
			c
		)
	)

	reg32 do(
		matchSyntax := method(str,
			c := str beginsWithSeq("reg32")
			c := c or (str beginsWithSeq("r/m32"))
			c
		)
	)

	fpureg do(
		matchSyntax := method(str,
			c := str beginsWithSeq("fpureg")
		//	c := c or (str beginsWithSeq("r/m32"))
			c
		)
	)

	mmreg do(
		matchSyntax := method(str,
			c := str beginsWithSeq("mm")
			c
		//	c or (str beginsWithSeq("r/m32"))
		)
	)

	symbols := Object clone do(
		appendProto(x86)

		_setSlot := getSlot("setSlot")
		_setSlot("setSlot",
		//	method(_setSlot(call evalArgAt(0),call evalArgAt(1)) )
			method(
			//	n := call evalArgAt(0)
			//	x := call evalArgAt(1)
			//	_setSlot(n,x)
				_setSlot("n", call evalArgAt(0))
				_setSlot(n,call evalArgAt(1)) setName(n)
			)
		)

		al := reg8 clone setId(0)
		cl := reg8 clone setId(1)
		dl := reg8 clone setId(2)
		bl := reg8 clone setId(3)
		ah := reg8 clone setId(4)
		ch := reg8 clone setId(5)
		dh := reg8 clone setId(6)
		bh := reg8 clone setId(7)
		ax := reg16 clone setId(0)
		cx := reg16 clone setId(1)
		dx := reg16 clone setId(2)
		bx := reg16 clone setId(3)
		sp := reg16 clone setId(4)
		bp := reg16 clone setId(5)
		si := reg16 clone setId(6)
		di := reg16 clone setId(7)
		eax := reg32 clone setId(0)
		ecx := reg32 clone setId(1)
		edx := reg32 clone setId(2)
		ebx := reg32 clone setId(3)
		esp := reg32 clone setId(4)
		ebp := reg32 clone setId(5)
		esi := reg32 clone setId(6)
		edi := reg32 clone setId(7)
		st0 := fpureg clone setId(0)
		st1 := fpureg clone setId(1)
		st2 := fpureg clone setId(2)
		st3 := fpureg clone setId(3)
		st4 := fpureg clone setId(4)
		st5 := fpureg clone setId(5)
		st6 := fpureg clone setId(6)
		st7 := fpureg clone setId(7)
		mm0 := mmreg clone setId(0)
		mm1 := mmreg clone setId(1)
		mm2 := mmreg clone setId(2)
		mm3 := mmreg clone setId(3)
		mm4 := mmreg clone setId(4)
		mm5 := mmreg clone setId(5)
		mm6 := mmreg clone setId(6)
		mm7 := mmreg clone setId(7)
		xmm0 := xmmreg clone setId(0)
		xmm1 := xmmreg clone setId(1)
		xmm2 := xmmreg clone setId(2)
		xmm3 := xmmreg clone setId(3)
		xmm4 := xmmreg clone setId(4)
		xmm5 := xmmreg clone setId(5)
		xmm6 := xmmreg clone setId(6)
		xmm7 := xmmreg clone setId(7)

		_setSlot("setSlot", getSlot("_setSlot"))

		cl do(
			matchSyntax := method(str,
				c := str beginsWithSeq("reg8")
				c := c or (str beginsWithSeq("r/m8"))
				c := c or (str beginsWithSeq("CL"))
				c
			)
		)


		squareBrackets := method(
			x86 parseMemRef( call sender, call message argAt(0), 32 ) // with bits=nil, fild/fist will take mem16 by default...
		)

		Specifier := Object clone do(
			ptr := nil
			init := method(
				ptr = Object clone do(
					squareBrackets := method(
						x86 parseMemRef( call sender, call message argAt(0), bits )
					)
				)
			)
			setBits := method ( bits, ptr bits := bits; self )
		)

		byte := Specifier clone setBits(8)
		word := Specifier clone setBits(16)
		dword := Specifier clone setBits(32)
		qword := Specifier clone setBits(64)
	)

	/*
		-- Instruction --
	*/

	Instruction := Object clone do(
		appendProto(x86)

		newSlot("name")
		newSlot("_name")
		newSlot("cc")
		newSlot("ops")
		newSlot("encoding")
		newSlot("emit", true)

		init := method(
			ops = List clone
		)

		copy := method(
			i := self clone
			i ops = ops clone
			i
		)

		asString := method( "#{name} #{ops asNakedString(\",\")}" interpolate )

		foreachRegister := method(
			s := call argAt(0) asString
			ops mapInPlace(o,

				if( o isKindOf(Register),

					call sender setSlot(s,o)
					call sender doMessage(call argAt(1))
					o = call sender getSlot(s)
					o

					,if( o isKindOf(MemoryReference),

						o base isKindOf(Register) ifTrue(
							call sender setSlot(s,o base)
							call sender doMessage(call argAt(1))
							o base = call sender getSlot(s)
						)
						o index isKindOf(Register) ifTrue(
							call sender setSlot(s,o index)
							call sender doMessage(call argAt(1))
							o index = call sender getSlot(s)
						)
						o
					,o)
				)
			)
		)

		resolveName := method(
			_name = name asUppercase

			cc = getConditionCode( _name )
			if ( cc,
				_name = _name slice(0, - cc at(1) size ) .. "cc"
			)

			mn := x86opcodes at(_name)

			if (mn isNil, Exception raise("unrecognized mnemonic : #{name}" interpolate))
		)

		isLocalJump := method( (_name == "Jcc") or (_name == "JMP") or ( (_name == "CALL") and (ops at(0) isKindOf(Label) ) ) )
		isGlobalJump := method( ( (_name == "CALL") and (ops at(0) isKindOf(Label) not) ) or (_name == "RET") )
		isJump := method( isLocalJump or isGlobalJump )

		resolve := method(ctx,
			resolveName
		//	isLocalJump ifFalse(
				ops mapInPlace(m, ctx doMessage(m) asOperand )
		//	)
		)

	//	newSlot("encodeTimer")
	//	Lobby encodeTimer := 0
	//	encode := method( start := Utils rdtsc; r := self _encode; encodeTimer = encodeTimer+(Utils rdtsc-start); encodeTimer println; r  )
//		encode := method( Utils getSlot("rdtsc") println; self _encode )
		//	encodeTimer = encodeTimer + chrono( r := self _encode )
		//	Lobby encodeTimer = Lobby encodeTimer + chrono( r := self _encode )
		//	encodeTimer println
		//	r

		//	start := Utils rdtsc; r := self _encode; encodeTimer = encodeTimer+crh(Utils rdtsc-start); encodeTimer println; r

		encode2 := method( ec measure( self _encode ) )
		encode := method(

		//	if (emit not, "emit not" println)
			if (emit not, return (""))
			if (encoding, return (encoding))
		//	if (encoding, return (encoding size / 2))
		//	isLabel ifTrue( encoding := "" )

		/*	if (ops select( isKindOf(CodeGeneration VirtualRegister) ) isEmpty not,
				return ("")
			)*/

			// to debug virtual registers
			ret := false
			foreachRegister(r, r isKindOf(CodeGeneration VirtualRegister) ifTrue( ret = true ) )
			ret ifTrue( return("") )

		//	self print

			mn := x86opcodes at(_name)
			mnops := mn map( at(0) split(",") )

		//	mnops foreach( println )

			mnops := mnops select(e, e size == ops size )
			if ( mnops isEmpty,
				mn flatten println
				Exception raise("operand number mismatch for #{name} ( #{ops size} )" interpolate)
			)

		//	ops print
			ops foreach(k,e,
		//	e print
			mnops selectInPlace(f, e matchSyntax( f at(k) ) ) )

	//		mnops foreach(println)

			mn := mn select(e, e at(0) == mnops at(0) ?asNakedString(",") ) at(0)

			if( mn isNil,
				mn := x86opcodes at(_name)
				mn reduce(..) println
				mnops := mn map( at(0) split(",") )
				mnops reduce(..) println

				ops println
				ops foreach(x, x println)

//				mnops := mnops select(e, e size == ops size )
//				mnops selectInPlace(f, e matchSyntax( f at(k) ) ) )

				Exception raise("couldn't encode mnemonic #{name}" interpolate)
			)

			code := mn at(mn size -2)

			i := 0;
			while(
				c := false
				c := c or( code at(i) >= "A" at(0) and code at(i) <= "F" at(0) )
				c := c or( code at(i) >= "0" at(0) and code at(i) <= "9" at(0) )
				c
				,
				i = i + 1
				if (i == code size, break)
			)

			icode := code slice(0,i)
			spec := code slice(i)

		//	code println
		//	mnops at(0) println

			if( spec beginsWithSeq("+r"),
			//	icode atPut(icode size, icode at(icode size) )
				byte := (icode slice(icode size-2) fromBase(16) + ops at(0) id) asString toBase(16) asUppercase alignRight(2,"0")
				icode := icode slice(0, icode size-2) .. byte
				spec := spec slice(2)
			)

			bang := false

			if( spec beginsWithSeq("+cc"),
			//	icode atPut(icode size, icode at(icode size) )
				byte := (icode slice(icode size-2) fromBase(16) + cc at(0)) asString toBase(16) asUppercase alignRight(2,"0")
				icode := icode slice(0, icode size-2) .. byte
				spec := spec slice(3)
			)


			if( spec beginsWithSeq("/"),

				spare := 0
				op := nil


				if (ops size == 1,
					spare = spec at(1) - "0" at(0)
					((spare > 7) or (spare < 0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )
					op = ops at(0)
				)

				if (ops size == 2,
			//	"x" asMutable atPut(0,spec at(1)) println
				//	(spec at(1) != "r") ifTrue( Exception raise("x86 Instruction/encode/Internal error") )

				// two ops
					c := false
					c := c cond( ops at(0) isKindOf(Register) and ops at(1) isKindOf(Register),
			//			"x" asMutable atPut(0,spec at(1)) println
				//		(spec at(1) != "r" at(0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )

						// quite ugly .... (assuming we took r/m,reg variant) -> fucks up imul
						spare = ops at(1) id
						op = ops at(0)
						fix := mnops at(0) at(0) beginsWithSeq("reg") // fix for reg,r/m (still ugly)
						fix = fix or (mnops at(0) at(0) beginsWithSeq("mm"))
						fix = fix or (mnops at(0) at(0) beginsWithSeq("xmm"))
						fix ifTrue(
							spare = ops at(0) id
							op = ops at(1)
						)
						// fix fixed reg ops (/#)
						(spec at(1) != "r" at(0)) ifTrue(
							spare = spec at(1) - "0" at(0)
					//		op = ops at(0)
						)

					//	mnops foreach(print)
					//	" " print
					//	println
					)
					c := c cond( ops at(0) isKindOf(MemoryReference) and ops at(1) isKindOf(Register),
						spare = ops at(1) id
						op = ops at(0)

		/*				ops at(0) println
						ops at(1) println
						_name println
						"x" asMutable atPut(0,spec at(1)) println
		*/
						// fix fixed reg ops (/#)
						(spec at(1) != "r" at(0)) ifTrue(
							spare = spec at(1) - "0" at(0)
					//		op = ops at(0)
						)


			//			"x" asMutable atPut(0,spec at(1)) println
			//			(spec at(1) != "r" at(0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )
					)
					c := c cond( ops at(0) isKindOf(Register) and ops at(1) isKindOf(MemoryReference),
						spare = ops at(0) id
						op = ops at(1)

						(spec at(1) != "r" at(0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )
					)
					c := c cond( ops at(0) isKindOf(Register) and ops at(1) isKindOf(Immediate),
						spare = spec at(1) - "0" at(0)
						op = ops at(0)

				//		"x" asMutable atPut(0,spec at(1)) println
						((spare > 7) or (spare < 0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )
					)
					c := c cond( ops at(0) isKindOf(MemoryReference) and ops at(1) isKindOf(Immediate),
						spare = spec at(1) - "0" at(0)
						op = ops at(0)

				//		"x" asMutable atPut(0,spec at(1)) println
						((spare > 7) or (spare < 0)) ifTrue( Exception raise("x86 Instruction/encode/Internal error") )
					)
				)

			//	if (spare,  )

				mod := nil
				rm := nil
				scale := nil
				index := nil
				base := nil

				if (op isKindOf(Register),
					mod = 3
					rm = op id
				)

				if (op isKindOf(MemoryReference),
					mod = 2
				//	op disp println
					if (op disp value <= 127 and op disp value >= -128, mod=1)
					if (op disp value == 0, mod=0)

					if(op index isNil and op base isNil,
						rm = 4
						scale = 0
						index = 4
						base = 4
					)
					if(op index isNil and op base isKindOf(Register),
						rm = 4
						scale = 0
						index = 4
						base = op base id
					)
					if(op index isKindOf(Register) and op base isKindOf(Register),
						rm = 4
						scale = op scale log10 / 2 log10
						index = op index id
						base = op base id
					)

					if (base isNil, Exception raise("x86 Instruction/encode/Internal error"))
				)

				if (rm isNil, Exception raise("x86 Instruction/encode/Internal error"))

				modrm_byte := rm + spare * 2**3 + mod * 2**6
				icode := icode .. modrm_byte asString toBase(16) asUppercase alignRight(2,"0")

				if (base,
					SIB_byte := base + index * 2**3 + scale * 2**6
					icode := icode .. SIB_byte asString toBase(16) asUppercase alignRight(2,"0")
				)

				if(mod==1,
					icode := icode .. op disp value asString toBase(16) asUppercase slice(-2) alignRight(2,"0")
			//	op disp println
			//	op disp value asString toBase(16) println

				)
				if(mod==2,
				//	icode := icode .. op disp value asString toBase(16) asUppercase alignRight(8,"0")

					hexv := op disp value asString toBase(16) asUppercase alignRight(8,"0")
					0 to(3) foreach(n, icode := icode .. hexv slice((3-n)*2,(3-n)*2+2)  ) // bswap
				)


				spec := spec slice(2)
			)
	/*
			if( spec beginsWithSeq("+r"),
			//	icode atPut(icode size, icode at(icode size) )
				byte := (icode slice(icode size-2) fromBase(16) + ops at(0) id) asString toBase(16) asUppercase alignRight(2,"0")
				icode := icode slice(0, icode size-2) .. byte
				spec := spec slice(2)
			)

			bang := false
			if( spec beginsWithSeq("+cc"),
			//	icode atPut(icode size, icode at(icode size) )
				byte := (icode slice(icode size-2) fromBase(16) + cc at(0)) asString toBase(16) asUppercase alignRight(2,"0")
				icode := icode slice(0, icode size-2) .. byte
				spec := spec slice(3)

			)*/

			/*
			if( spec beginsWithSeq("rb"),
				if (ops last isKindOf(Label)) then(
					spec := spec slice(2)
				) else (
					Exception raise("invalid operand [rb -> label]")
				)
			)*/

			if( spec beginsWithSeq("rd") or spec beginsWithSeq("rw/rd"),

				icode := icode .. "" alignRight(8,"x") // patch that later ~

				if (spec beginsWithSeq("rw/rd"),
					spec := spec slice(5),
					spec := spec slice(2)
				)

			/*	if (ops last isKindOf(Label)) then(
					patchAt = icode size
					patch = method(labels,
						icode2 := encoding slice(0, patchAt)
				//		icode2 println
				//		ip println
						hexv := (labels at(ops last name) - (ip + 4 + icode2 size / 2)) asString toBase(16) asUppercase alignRight(8,"0")
						0 to(3) foreach(n, icode2 := icode2 .. hexv slice((3-n)*2,(3-n)*2+2) ) // bswap
				//		icode2 println
						encoding = icode2
					)

					icode := icode .. "xyxyxyxy" //ops last value asString toBase(16) asUppercase alignRight(8,"0")

					if (spec beginsWithSeq("rw/rd"),
						spec := spec slice(5),
						spec := spec slice(2)
					)
				) else (
					Exception raise("invalid operand [rd -> label]")
				)
				*/
			)

			if( spec beginsWithSeq("id"),
				if (ops last isKindOf(Immediate)) then(
		//			hexv := ops last value asString toBase(16) asUppercase alignRight(8,"0")
			//		ops last value println
					high := (ops last value / 2**16) asString toBase(16)
					low := (ops last value % 2**16) asString toBase(16)
					hexv := (high .. low) asUppercase alignRight(8,"0")
			//		hexv println
					0 to(3) foreach(n, icode := icode .. hexv slice((3-n)*2,(3-n)*2+2)  ) // bswap
					spec := spec slice(2)
				) else (
					Exception raise("invalid operand [id -> imm]")
				)
			//	c := c elseif(ops last isKindOf(Label))
			)

			if( spec beginsWithSeq("ib"),
				if (ops last isKindOf(Immediate)
					icode := icode .. ops last value asString toBase(16) asUppercase alignRight(2,"0")
					spec := spec slice(2)
				) else (
					Exception raise("invalid operand [ib -> imm]")
				)
			)


			if (spec size > 0, Exception raise("internal error : unrecognized mods : #{spec}" interpolate) )

		//	icode .. (spec) println
		//	mnops := mn map( at(0) split(",") )
		//	mnops at(0) asNakedString(",") println

			if( mnops isEmpty, Exception raise("parse error, unknow encoding") )

		//	"\n" print
		//	encoding size / 2

			encoding = icode
			encoding

		)

//		test := method( self print; nil )
		test := method( encoding print )

	)

	parseMemRef := method( ctx, operand, bits,

	//	operand println

		op := MemoryReference clone setBits(bits)

		op disp = 0

		one := ctx doMessage(operand clone setAttached) asOperand
		if ( one isKindOf(Register), op base = one )
		if ( one isKindOf(Immediate),  op disp = one )

		two := nil
		three := nil
		if (operand attached,
			c := operand attached name == "+"
			c := c or( operand attached name == "-" )
			c ifTrue( two = ctx doMessage(operand attached argAt(0) clone setAttached) asOperand )
			if (operand attached attached,
				c := operand attached attached name == "+"
				c := c or( operand attached attached name == "-" )
				c ifTrue( three = ctx doMessage(operand attached attached argAt(0) clone setAttached) asOperand )
			)
		)

		if ( two isKindOf(Register),
			op index = two
			if ( (operand attached argAt(0) attached isNil not) and (operand attached argAt(0) attached name == "*" ),
				op scale = ctx doMessage(operand attached argAt(0) attached argAt(0)),
				op scale = 1
			)
		)

		if ( two isKindOf(Immediate),
			if( operand attached name == "+",
				op disp = two,
				op disp = -two)
		)

		if ( three isKindOf(Immediate),
			if( operand attached attached name == "+",
				op disp = three,
				op disp = -three)
		)

		op base = op base ?asOperand
		op index = op index ?asOperand
		op disp = op disp ?asOperand

		op
	)

	// protect these ops from operator translations
	operator_revert := list("and","or")


//	asmProxy := method(msg, code,  parseAsm( argAt(1) , call sender, evalArgAt(0) ) )

	parseAsm := method(msg, ctx, listing,

		k := 0
	//	listing := asmCode clone

		msg = msg clone

		m := msg argAt(0)

		loop(

			// revert some operator translations
			operator_revert foreach(op, nil;
				if(m name == op,
			//	m println
				m setAttached(m argAt(0)) setArguments(list())
			//	m println
				)
			)

			instr := Instruction clone setName(m name)

			if (m attached isNil,
				// end of line
				if (m next isNil, break) // end of message

				m = m next
				instr resolve(ctx); listing push(instr); continue;
			)

			instr ops push( m attached clone )

			if (m next, m = m next; instr resolve(ctx); listing push(instr); continue) // next line

			k = k + 1;
//			if (k >= msg argCount, Exception raise("parse error, nothing after \",\""))
			if (k >= msg argCount, break)
			m = msg argAt(k)

			instr ops push( m clone setNext )

			if (m next isNil, break)
			m = m next

			instr resolve(ctx); listing push(instr)
		)

		instr resolve(ctx); listing push(instr) // last line

	//	listing print

	//	listing foreach(i, (i name=="and") ifTrue(i println) )

	//	listing foreach( resolve(ctx) )
	//	listing
	)

); // x86 do
