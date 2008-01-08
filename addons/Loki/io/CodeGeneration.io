/*
	-- Loki -- BasicBlock/ControlFlowGraph, Register Allocation --
*/

doFile("Assembler.io")

CodeGeneration := Object clone
CodeGeneration do(
	appendProto(x86)

	CodePointer := Object clone do(
		newSlot("localBlock")	// block
		newSlot("instruction") // instruction id

		instructionId := method(
			if(instruction isNil, -1, localBlock code indexOf(instruction))
		)

		asString := method(
			"#{localBlock id}.#{instructionId + 1}" interpolate
		)
		greater := method(cp,
			(self localBlock id == cp localBlock id) ifTrue(
				return( self instructionId > cp instructionId )
			)
			self localBlock id > cp localBlock id
		)

		greaterOrEqual := method(cp,
			(self localBlock id == cp localBlock id) ifTrue(
				return( self instructionId >= cp instructionId )
			)
			self localBlock id > cp localBlock id
		)

		lower := method(cp, greaterOrEqual(cp) not )
		lowerOrEqual := method(cp, greater(cp) not )

		setSlot(">", getSlot("greater"))
		setSlot(">=", getSlot("greaterOrEqual"))
		setSlot("<", getSlot("lower"))
		setSlot("<=", getSlot("lowerOrEqual"))
	)

	VirtualRegister := Register clone do(

		/*
			subclassed by specific arch regs

			contains LVA data

		*/

		appendProto(CodeGeneration)

		newSlot("firstUse")
		newSlot("lastUse")
		newSlot("concrete")			// allocated ressource ( register or memory reference )
		newSlot("firstUse")
		newSlot("noSpill", false)

		isUsed := method( firstUse isNil not )

		useAtIfFirstDo := method(b, i, msg,
			isUsed ifFalse(
				firstUse = CodePointer clone setLocalBlock(b) setInstruction(i)
				call sender doMessage(msg)
			)
			lastUse = CodePointer clone setLocalBlock(b) setInstruction(i)
		)

		aliveAtBlock := method(b,
			isUsed ifFalse( return false )
			(firstUse localBlock id >= b id) ifTrue( return false )
			(lastUse localBlock id < b id) ifTrue( return false )
			true
		)

		LVAInfo := method(
//			("<" .. self name .. "> live interval : [" .. firstUse .. " " .. lastUse .. "]")
			("<" .. self name .. "> [" .. firstUse .. " " .. lastUse .. "]")
		)

		asString := method(
			"<#{self name}>" interpolate
		)

	)


	// unsupported by the allocator - 8/16 bit registers aliasing the 32 bit register file
//	setSlotWithType("vreg8", VirtualRegister clone)
//	setSlotWithType("vreg16", VirtualRegister clone)

	// unsupported by the allocator - x87 stack register file is a mess
//	setSlotWithType("vfpureg", VirtualRegister clone)

	setSlotWithType("vreg32", VirtualRegister clone)
	setSlotWithType("vmmreg", VirtualRegister clone)
	setSlotWithType("vxmmreg", VirtualRegister clone)


	BasicBlock := Object clone
	BasicBlock do(

		/*
			target for jumps
			ends with : JMP, Jcc, CALL, RET

			CFG utils
			jump instruction (code last...) utils

		*/

		newSlot("tail")
		newSlot("code")
		newSlot("codeBytes")
		newSlot("next")
		newSlot("jump")
		newSlot("ubranch", false)	// unconditional branch
		newSlot("id")
		newSlot("ip")

		// internal
		init := method( code = List clone; tail = self )

		asString := method(
			"-> block #{id}" interpolate
		)

		asStringInfos := method(
			str := "#{self}\n" interpolate
			if (code isEmpty,
				str = str .. "empty block : no code\n",
				code foreach(i, str = str .. "#{i encode alignLeft(20)} #{i asString alignLeft(20)}\n" interpolate )
			)
			jump isNil ifFalse( str = str .. "-- jump #{jump}\n" interpolate )
			ubranch ifFalse( str = str .. "-- next #{next}\n" interpolate )
			str = str .. "\n"
		)

		pushCode := method(k,
			tail code push(k)
			k isJump ifTrue(
				if (k isLocalJump,
					tail jump = k ops at(0) target
				//	k ops atPut(0,0)
					if (k _name == "JMP", tail ubranch = true)
				)
				tail = CodeGeneration BasicBlock clone after(self)
			)
		)

		asm := method(
			jump isNil ifFalse( Exception raise("block already has a jump ...") )
			localCode := List clone
			x86 parseAsm(call message, call sender, localCode)
			// very important to respect LIFO here : x86 parseAsm is going to resolve references
			localCode foreach(i, pushCode(i))
			self
		)

		// don't use jumps on these ...
		putBefore := method(
			localCode := List clone
			x86 parseAsm( call argAt(1) , call sender, localCode)
			code insertBefore( localCode at(0), call evalArgAt(0)  )
			self
		)

		putAfter := method(
			localCode := List clone
			x86 parseAsm( call argAt(1) , call sender, localCode )
			code insertAfter( localCode at(0), call evalArgAt(0)  )
			self
		)

		after := method(before,
			t := before tail
			// remove empty tails
			if (before tail code isEmpty and before tail != before,
				t = before
				loop(
					if (t next code isEmpty, break);
					t = t next
				)
			)
			t next = self
			self setId( t id + 1 )
			self
		)


		encode := method(
			if (codeBytes, return(codeBytes))
			if (code isEmpty, "",
				codeBytes = code map( encode ) reduce(..)
			)
		)

		patch := method(

	//	"patch" println
	//	"patch jump = #{jump}" interpolate println

			jump isNil ifFalse(
				patchAt := (codeBytes size)/2 - 4
				icode2 := codeBytes slice(0, patchAt*2)
				hexv := (jump ip - (ip + patchAt + 4)) asString toBase(16) asUppercase alignRight(8,"0")
				0 to(3) foreach(n, icode2 := icode2 .. hexv slice((3-n)*2,(3-n)*2+2) ) // bswap
				codeBytes = icode2
			)
		)

		asOperand := method(
			x86 Label clone setTarget(self)
		)

	)

	ControlFlowGraph := Object clone do(

		/*
			function level
			handles stack and arguments

			build with explicit CFG
			performs Register Allocation
			uses Assembler
			handles binary code

			code : LVA, RA, binutils

		*/


		appendProto(CodeGeneration)
		newSlot("top")
		newSlot("target")
		newSlot("vregList") // to debug

		// internal
//		newSlot("stack")
//		newSlot("arguments")

		appendProto(x86 symbols)

		init := method(
			top = BasicBlock clone
			top setId(0)
			target = BasicBlock clone after(top)
		)

		foreach := method(
			s := call argAt(0) asString
			b := top
			loop(
				call sender setSlot(s,b)
				call sender doMessage(call argAt(1))
				b = b next
				b isNil ifTrue( break )
			)
		)

		refArgAt := method(n,

			/*
				x86 stack layout : (after function prolog)

					...
					arg3		-- ebp + 16
					arg2		-- ebp + 12
					arg1		-- ebp + 8
					retptr	-- ebp + 4
					old esp  -- ebp + 0
					local1	-- ebp - 4
					local2	-- ebp - 8
					..
					localN	-- esp + 0

			*/

			// 4 bytes for last frame pointer, 4
			disp := (n*4+8);
			[ebp+disp]
		)

		registerAllocation := method(

			diagStr := Sequence clone

			diagStr appendSeq("Start Register Allocation...\n") //println
			diagStr appendSeq("PASS 1\n") //println // PASS 1 : analysis

			vregList := List clone
			registerNumber := 1

			foreach(b,
				b code foreach(i,vregList

					// range scan
					i foreachRegister(r,
						r isKindOf(VirtualRegister) ifTrue(
							r isUsed ifFalse( vregList push(r)  )
							r useAtIfFirstDo( b, i,
								message(
									r name isNil ifTrue(
										r setName("r#{registerNumber}" interpolate)
									)
									registerNumber = registerNumber + 1
								)
							)
						)
					)
				)

				if ((b jump isNil not) and ( b jump id <= b id),
					// handle backward jump (loops)

					vregList foreach(r,
						if (r aliveAtBlock(b jump),
							// for all regs that are live in the context of the target
							// keep alive until next block
							r useAtIfFirstDo(b next,nil, message(Exception raise("internal error")))
							diagStr appendSeq("analysis : keep #{r} alive until block #{b next id}\n" interpolate)
						)
					)
				)
			)

			diagStr appendSeq(asStringInfos)
			vregList foreach(vreg, diagStr appendSeq(vreg LVAInfo .. "\n") )

			diagStr appendSeq("PASS 2\n") // println // PASS 2 : allocation

			// esp, ebp, edx reserved
			vreg32Bank := list(eax, ecx, esi, edi, ebx)
			vmmregBank := list(mm0, mm1, mm2, mm3, mm4, mm5, mm6) // mm7 reserved
			vxmmregBank := list(xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6) // xmm7 reserved
			// potential improvement : we can track register free positions to avoid random blind spills when we lock
			stackDepth := -4
			vreg32Active := List clone
			vmmregActive := List clone

			vreg32List := vregList select( isKindOf(vreg32) )
			vmmregList := vregList select( isKindOf(vmmreg) )
			vxmmregList := vregList select( isKindOf(vxmmreg) )
			removeSlot("vregList")

			1 to(3) foreach(rt,

				(rt == 1) ifTrue( vregList := vreg32List; registers := vreg32Bank )
				(rt == 2) ifTrue( vregList := vmmregList; registers := vmmregBank )
				(rt == 3) ifTrue( vregList := vxmmregList; registers := vxmmregBank )

				active := List clone

				vregList foreach(vreg,

			//		registers foreach(r, (r .. " ") print )
			//		"" println

			//		vreg LVAData println

					// expire old intervals
					active selectInPlace(a,
						// still alive ?
						c := (a lastUse >= vreg firstUse)
						c ifFalse( registers push(a concrete) )
						c ifFalse( diagStr appendSeq("allocation : #{a asString alignLeft(10)} allocated to" interpolate alignLeft(40) .. a concrete .."\n") )
						c
					)

					if ((registers isEmpty) or ((vreg concrete isNil not) and (registers contains(vreg concrete) not)),
						// spill

				/*		vreg println
						registers foreach(r, (r .. " ") print )
						"" println
				*/

				//		active foreach(LVAInfo print); "" println

						active sortInPlaceBy( block(a,b, a lastUse < b lastUse ) )
						spill := active last

				//		active foreach(LVAInfo print); "" println

		//				"allocator debug : #{spill} lastUse = #{spill lastUse} /  #{vreg} lastUse = #{vreg lastUse} " interpolate println
		//				"allocator debug : #{spill lastUse > vreg lastUse} / #{vreg noSpill}" interpolate println

						if ( (spill lastUse > vreg lastUse) or (vreg noSpill)) then(
							if (vreg concrete isNil) then(

						/*		spill = active select(r, r noSpill not) last
								vreg concrete = spill concrete
								active remove(spill)
								active push(vreg)
						*/

								vreg concrete = spill concrete
								active pop // pop spilled reg
								active push(vreg)

							) else (
								spill = active select(r, r concrete == vreg concrete) at(0)
								active remove(spill) // pop spilled reg
								active push(vreg)
							)

							(spill noSpill) ifTrue(
								Exception raise(
									m := active map(n, n slotSummary .. "\n") reduce(..)
									m println
									spill slotSummary println
									"CFG/Register Allocation/Allocation Error : register collision : #{spill} & #{vreg}" interpolate
								)
							)

						) else(
							spill = vreg
						)

						spill concrete = [ebp+stackDepth]
						stackDepth = stackDepth - 4

						diagStr appendSeq("allocation : #{spill asString alignLeft(10)} spilled to" interpolate alignLeft(40) .. spill concrete .."\n")

					) else (
						// allocate a register

						if (vreg concrete isNil,
							vreg concrete = registers pop
						) else (
							registers remove(vreg concrete)
						)
						active push(vreg)
					)
				)
			)

			diagStr appendSeq("PASS 3\n") // println // PASS 3 : translation

//			cfg := CodeGeneration ControlFlowGraph clone
			cfg := self
			lastBlock := nil
			foreach(b,
				lastBlock = b

			/*	if (lastBlock,
					newBlock := BasicBlock clone after(lastBlock)
					lastBlock = newBlock
				,
					lastBlock = cfg top
				)*/

			//	oldcode := b code clone

			//	oldcode println

				b code foreach(i,

				//	i = i copy

					resolveMemory := list(0)
					(i ops size > 1) ifTrue( resolveMemory = list(0,1) )

					resolveMemory = resolveMemory map(n,
						d := 0
						i ops at(n) isKindOf(VirtualRegister) and( i ops at(n) concrete isKindOf(MemoryReference) ) ifTrue(
							d = 1
						)
						i ops at(n) isKindOf(MemoryReference) ifTrue(
							d = 1
							i ops at(n) base isKindOf(VirtualRegister) ifTrue(
								(i ops at(n) base concrete isKindOf(MemoryReference)) ifTrue( d = 2 )
							)
						)
						d
					)

				//	resolveMemory = list()

					flags := resolveMemory map(n, n asString .. ",") reduce(..) slice(0,-1)


					// one operand

					(resolveMemory == list(0) or resolveMemory == list(1)) ifTrue(
						nil
					)

					(resolveMemory == list(2)) ifTrue(

						r := i ops at(0)
						memRef := r base concrete
					//	disp := r disp

						diagStr appendSeq("translation : load/store virtual indirection #{r} #{memRef} [#{flags}]\n" interpolate)

						b putBefore(i, asm(mov edx, memRef ))
						r base = edx
					)

					// two operands

					(resolveMemory == list(0,0) or resolveMemory == list(1,0) or resolveMemory == list(0,1)) ifTrue(
					//	newBlock pushCode(i)
						nil
					)

					(resolveMemory == list(1,1)) ifTrue(

						r := i ops at(0)
						memRef := nil
						r isKindOf(VirtualRegister) ifTrue(
							memRef = r concrete
							diagStr appendSeq("translation : load/store virtual register #{r} #{memRef} [#{flags}]\n" interpolate)
						)
						i ops at(0) isKindOf(MemoryReference) ifTrue(
							memRef = r
							diagStr appendSeq("translation : load/store memory reference #{r} [#{flags}]\n" interpolate)
						)

						// no need to load if we overwrite
						( i _name == "MOV") ifFalse(
					//		newBlock asm( mov edx, memRef )
							b putBefore(i, asm(mov edx, memRef ))
						)

						i ops atPut(0, edx)
					//	newBlock pushCode(i)
					//	newBlock asmAfter(i, message(mov memRef, edx ))
						b putAfter(i, asm(mov memRef, edx ))
					)

//					((resolveMemory size > 1) and (resolveMemory max == 2) and (i _name beginsWithSeq("MOV") not)) ifTrue(
					((resolveMemory size > 1) and (resolveMemory max == 2) and (i _name != "MOV") and (i _name != "MOVZX")) ifTrue(
						i _name println
						Exception raise("CFG/Register Allocation/Illegal Indirection ( vreg indirection legal only with MOV instructions )" interpolate )
					)
					(resolveMemory == list(2,1)) ifTrue(
						Exception raise("CFG/Register Allocation/Illegal indirection ( force write pointers in registers )" interpolate )
					)
					(resolveMemory == list(2,2)) ifTrue(
						Exception raise("CFG/Register Allocation/Illegal indirection ( addressing memory on both operands ... )" interpolate )
					)

					(resolveMemory == list(0,2)) ifTrue(

						r := i ops at(1)
						memRef := r base concrete
			//			disp := r disp

						diagStr appendSeq("translation : load/store virtual indirection #{r} #{memRef} [#{flags}]\n" interpolate)

						b putBefore(i, asm(mov edx, memRef ))
						r base = edx
//						i ops atPut(1, [edx+disp])
					//	newBlock pushCode(i)
					)

					(resolveMemory == list(2,0)) ifTrue(

						r := i ops at(0)
						memRef := r base concrete
				//		disp := r disp

						diagStr appendSeq("translation : load/store virtual indirection #{r} #{memRef} [#{flags}]\n" interpolate)

						b putBefore(i, asm(mov edx, memRef ))
						r base = edx
					//	i ops atPut(0, [edx+disp])
					//	newBlock pushCode(i)
					)

					(resolveMemory == list(1,2)) ifTrue(

						r := i ops at(1)
						memRef := r base concrete
			//			disp := r disp

						diagStr appendSeq("translation : load/store virtual indirection #{r} #{memRef} [#{flags}]\n" interpolate)

						b putBefore(i, asm(mov edx, memRef ))
						r base = edx
						b putBefore(i, asm(mov edx, r ))
						i ops atPut(1, edx)
					//	newBlock pushCode(i)
					)

					i foreachRegister(r, if(r isKindOf(VirtualRegister), r = r concrete) )
				)
			)

			diagStr appendSeq("Register Allocation Ok.\n") // Finished ^^

			// the post/prolog are big, but CFunction activation is an order of magnitude slower anyway ...

			cfg top asm(
				push ebp
				mov ebp, esp
				sub esp, (-4-stackDepth)	// allocate automatic local stack space - users can go deeper in the stack manually
				push edi
				push esi
				push ebx
			)

			lastBlock asm(
				pop ebx
				pop esi
				pop edi
				mov esp, ebp
				pop ebp
				ret
			)

			diagStr appendSeq(asStringInfos)

			diagStr
		)

		optimise := method(

			foreach(b,
				b code foreach(i,

					if (i _name == "MOV" and i ops at(0) == i ops at(1),
					//	i println
						i setEmit(false)
					//	i setName("nilMOV")
					)

				)
			)


		)


		asStringInfos := method( s := ""; foreach(b, s = s .. b asStringInfos ); s )

		scan := method(
			ip := 0
			foreach(b,
				b ip = ip
				ip = ip + (b encode size / 2)
			)
		)

		patch := method( foreach(b, b patch ) )

		encode := method(
			scan
			patch
			codeBytes := ""
			foreach(b,
				codeBytes = codeBytes .. b encode
			)
			codeBytes
		)

	)

); // CodeGeneration do
