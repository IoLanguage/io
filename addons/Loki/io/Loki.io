/*
	-- Loki -- x86 Loki primitives --
*/

doFile("CodeGeneration.io")

// a method that returns "self"
Object procedure := method(
	msg := call message arguments last;
	while( msg next, msg = msg next );
	msg setNext(message(self));
	call delegateToMethod(self, "method");
)

Loki := Object clone
Loki do(

	Primitive := Object clone do(
		appendProto(CodeGeneration)
		appendProto(Loki)
		newSlot("contextProxy")

		// proxy interface
		setProxy := procedure(
			(call argCount == 1) ifFalse( last := call sender ) ifTrue( last := call evalArgAt(0) )
			// get the top of the tree
			while( last hasSlot("contextProxy"), last = last contextProxy )
			contextProxy = last
		)
		proxyBlock := method( contextProxy proxyBlock )
		setProxyBlock := method(nlb, contextProxy setProxyBlock(nlb) )
	)

	Loop := Primitive clone do(

		newSlot("initMsg")
		newSlot("incMsg")
		newSlot("condMsg")
		newSlot("bodyMsg")
		newSlot("hasEntryCondition")
		newSlot("index")
		newSlot("simpleLoop", false)

		cloneIfProto := procedure( self protos at(0) isKindOf(Loop) ifFalse( self = Loop clone ) )

		as := method( cloneIfProto setInitMsg( call argAt(0) ) )
		by := method( cloneIfProto setIncMsg( call argAt(0) ) )

		toZero := procedure(
			setSimpleLoop(true)
			setHasEntryCondition(true)
		)

		// shortcut with C syntax
		for := procedure(

	//		"in for()" println

			msg := call argAt(0)
			self = cloneIfProto
			setInitMsg( msg clone setNext )
			setCondMsg( msg clone next setNext )
			setIncMsg( msg clone next next )
			setHasEntryCondition(true)
		)

		while := procedure(
			self = cloneIfProto
			setCondMsg( call argAt(0) )
			bodyMsg isNil ifFalse( setProxy(call sender) compile ) ifTrue( setHasEntryCondition(true) )
			index
		)

		with := procedure(
			self = cloneIfProto
			setBodyMsg( call argAt(0) )
			condMsg isNil ifFalse( setProxy(call sender) compile ) ifTrue( setHasEntryCondition(false) )
			index
		)

		compile := method(ctx,

	/*		initMsg println
			bodyMsg println
			incMsg println
			condMsg println

		//	contextProxy println
			self println
	*/
			if ( initMsg, index = contextProxy doMessage( initMsg ) )

	//		index println

			loopEntry := BasicBlock clone
			loopEnd := BasicBlock clone

			hasEntryCondition ifTrue(
				c := contextProxy doMessage( condMsg )

			//	c println

				proxyBlock asm(
					cmp c, 0
					je loopEnd
				)
			)

			loopEntry after(proxyBlock)
			setProxyBlock(loopEntry)
			contextProxy doMessage( bodyMsg )
			if ( incMsg, contextProxy doMessage( incMsg ) )

			simpleLoop ifTrue(
				proxyBlock asm(
					dec index
					jnz loopEntry
				)
			) ifFalse(
				c := contextProxy doMessage( condMsg )
				proxyBlock asm(
					cmp c, 0
					jne loopEntry
				)
			)

			loopEnd after(proxyBlock)
			setProxyBlock(loopEnd)
		)
	)

	// Double : two integers for x86-32, double-precision ieee-754 floating point values
	// hack to get the timestamp counter & push fp64 values on stack
	// i'm not willing to fully support fpu regs anyway, x87 is a mess
	// all operations are done using the stack and references won't work
	Double := Primitive clone
	double := method( Loki Double clone setProxy(call sender) initRegs )
	Double := Primitive clone do(
		appendProto(x86 symbols)

		newSlot("high")
		newSlot("low")
		initRegs := procedure( high = vreg32 clone; low = vreg32 clone )

		rdtsc := procedure(
			reax := vreg32 clone setConcrete(eax) setNoSpill(true)
			redx := vreg32 clone setNoSpill(true)	// redx != edx ^^
			proxyBlock asm(
				mov reax, reax
				rdtsc
				mov reax, reax
				mov redx, edx
				mov low, reax
				mov high, redx
			)
		)

		asFloat64 := method(
			r := double
			proxyBlock asm(
				push high
				push low
				fild qword ptr [esp]
				fst qword ptr [esp]
				pop r low
				pop r high
			)
			r
		)

		push := method(
			proxyBlock asm(
				push high
				push low
			)
		)
		pushBytes := 8			// number of bytes we push on stack, used by callers to manage the stack
	)

	Int64 := Primitive clone
	int64 := method( Loki Int64 clone setProxy(call sender) initRegs )
	Int64 := Primitive clone do(
		appendProto(x86 symbols)
		newSlot("name")
		newSlot("value")

		asOperand := method(
			value hasSlot("name") ifTrue(
				value name isNil ifTrue(value setName(name))
			)
			value
		)
		initRegs := procedure( setValue(vmmreg clone) )

		// mov, memory store/load

		setTo := procedure(v,
			(v isKindOf(Number))	ifTrue( r := vreg32 clone;	proxyBlock asm( mov r, v; movd self, r ) )
			(v isKindOf(Integer))	ifTrue( proxyBlock asm( movd self, v ) )
			(v isKindOf(Int64))			ifTrue( proxyBlock asm( movq self, v ) )
		)

		setSlot("<-", getSlot("setTo"))


		// inplace arithmetic operations

		setSlot("+=", method(v, proxyBlock asm( paddq self, v ) ))
		setSlot("-=", method(v, proxyBlock asm( psubq self, v ) ))	// SSE2 ?


		// arithmetic operations

		setSlot("+", method(v, r := int64; proxyBlock asm( movq r, value; paddq r, v ); r ))
		setSlot("-", method(v, r := int64; proxyBlock asm( movq r, value; psubq r, v ); r ))

		// inplace binary/logic operations

		setSlot("&=", procedure(v, proxyBlock asm( pand	value, v ); ))
		setSlot("|=", procedure(v, proxyBlock asm( por	value, v ); ))
		setSlot("^=", procedure(v, proxyBlock asm( pxor	value, v ); ))

		// binary/logic operations

		setSlot("<<", method(v, r := int64; proxyBlock asm( movq r, value; psllq	r, v ); r ))
		setSlot(">>", method(v, r := int64; proxyBlock asm( movq r, value; psrlq	r, v ); r ))
		setSlot("&",  method(v, r := int64; proxyBlock asm( movq r, value; pand		r, v ); r ))
		setSlot("|",  method(v, r := int64; proxyBlock asm( movq r, value; por		r, v ); r ))
		setSlot("^",  method(v, r := int64; proxyBlock asm( movq r, value; pxor		r, v ); r ))


		push := method(
			proxyBlock asm(
				sub esp, 8
				movq [esp], value
			)
		)
		pushBytes := 8			// number of bytes we push on stack, used by callers to manage the stack
	)

	// Void : native integer - naked
	Void := Primitive clone
	void := method( Void clone setProxy(call sender) initRegs )
	Void do(
		newSlot("name")
		newSlot("value")

		asOperand := method(
			value hasSlot("name") ifTrue(
				value name isNil ifTrue(value setName(name))
			)
			value
		)
		initRegs := procedure( setValue(vreg32 clone) )
		castAs := method(type,
			type isKindOf(Void) ifFalse(
				Exception raise("Loki : #{self type} -> #{type} is an invalid cast. #{type} isn't compatible with Void" interpolate)
			)
			type clone setProxy setValue(value)
		)
	)

	// Integer : native integer - can do everything (for now. implement domain-oriented void mixins later)
	Integer := Void clone
	int := procedure(
		self := Integer clone setProxy(call sender) initRegs
		(call argCount > 0) ifTrue( self setValue(call evalArgAt(0)))
	 )
	Integer do(
		appendProto(x86 symbols)

		// branches

		ifTrue := method(

			end := BasicBlock clone
			proxyBlock asm(
				cmp self, 0
				je end
			)
			setProxyBlock(proxyBlock tail)

			call evalArgAt(0)

			end after(proxyBlock)
			setProxyBlock(end)
		)

		ifFalse := method(

			end := BasicBlock clone
			proxyBlock asm(
				cmp self, 0
				jne end
			)
			setProxyBlock(proxyBlock tail)

			call evalArgAt(0)

			end after(proxyBlock)
			setProxyBlock(end)
		)


		// mov, memory store/load

		setTo := procedure(v,
			(v isKindOf(Number))	ifTrue( proxyBlock asm( mov	self, v ) )
			(v isKindOf(Integer))	ifTrue( proxyBlock asm( mov	self, v ) )
			(v isKindOf(Int64))			ifTrue( proxyBlock asm( movd	self, v ) )
		)

		push := method(

		//	self println

			proxyBlock asm( push self )
		)
		pushBytes := 4

		setToReturnValue := method( proxyBlock asm( mov eax, self ) )

		// ptr is void*, 32 bits for x86-32

		ptrAtGet := method(n,
			r := int
			proxyBlock asm( mov r, [self+n] )
			r
		)

		ptrAtPut := method(n, r,
			proxyBlock asm( mov [self+n], r )
		)

		ptrGet := method(
			r := int
			proxyBlock asm( mov r, [self] )
		//	r println
			r
		)

		ptrPut := method(r,
			proxyBlock asm( mov [self], r )
//			proxyBlock asm( mov [eax], r )
		)

		uint8Get := method(

		//	"->> #{value} <<-" interpolate println

			r := int
			rlock := vreg32 clone setNoSpill(true)
			proxyBlock asm(
				movzx rlock, byte ptr [self]
				mov r, rlock
			)
			r
		)

		uint8Put := method(r,
			rlock := vreg32 clone setNoSpill(true)
			proxyBlock asm(
//				mov		rlock, self
				mov		rlock, self
				mov		edx, r
//				mov		byte ptr [rlock], r
				mov		byte ptr [rlock], dl
			)
			self
		)

		int64Get := method(
			r := int64
			proxyBlock asm( movq r, qword ptr [self] )
			r
		)

		int64Put := method(r,
			proxyBlock asm( movq qword ptr [self], r )
		)

		setSlot("<-", getSlot("setTo"))

		// conversion

		int32FromFloat64Get := method(
			r := int
			proxyBlock asm(
				fld qword ptr [self]
				sub esp, 4
				fistp dword ptr [esp]
				pop r
			)
			r
		)

		asFloat64 := method(

	//	"bleh" println

			r := double
			proxyBlock asm(
				sub esp, 8			// alloc a qword
				mov [esp], self
				fild [esp]
				fst qword ptr [esp]
				pop r low
				pop r high
			)

	//		proxyBlock asStringInfos println

			r
		)


		// comparisons

		doCompare := method(instr, v,

		//	v println
		//	self println

			r := int
			rlock := vreg32 clone setNoSpill(true)
			msg := message(
				proxyBlock asm(
					cmp		self, v
					SETcc	dl
					movzx rlock, dl
					mov		r, rlock
				)
			)
			msg attached arguments at(1) next setName(instr) // patch SETcc

		//	msg println

			doMessage( msg )
			r
		)

		setSlot("==", method(v, doCompare("sete", v)))
		setSlot("<", method(v, doCompare("setl", v)))
		setSlot("<=", method(v, doCompare("setle", v)))
		setSlot(">", method(v, doCompare("setg", v)))
		setSlot(">=", method(v, doCompare("setge", v)))


		// inplace arithmetic operations

		setSlot("++", method(v, proxyBlock asm( inc self ) ))
		setSlot("--", method(v, proxyBlock asm( dec self ) ))
		setSlot("+=", method(v, proxyBlock asm( add self, v ) ))
		setSlot("-=", method(v, proxyBlock asm( sub self, v ) ))


		// arithmetic operations

		div := method(v,
			r := int;
			reax := vreg32 clone setConcrete(eax) setNoSpill(true)
			proxyBlock asm(
				mov reax, value
				mov edx, 0
				idiv v
				mov r, reax
			)
			r
		)

		mod := method(v,
			r := int;
			reax := vreg32 clone setConcrete(eax) setNoSpill(true)
			redx := vreg32 clone setNoSpill(true)	// redx != edx ^^
			proxyBlock asm(
				mov reax, value
				mov edx, 0
				idiv v
				mov reax, reax
				mov redx, edx
				mov r, redx
			)
			r
		)

		setSlot("+", method(v, r := int; proxyBlock asm( mov r, value;	add r, v ); r ))
		setSlot("-", method(v, r := int; proxyBlock asm( mov r, value;	sub r, v ); r ))
		setSlot("*", method(v, r := int; proxyBlock asm( mov r, value;	imul r, v ); r ))
		setSlot("/", getSlot("div"))
		setSlot("%", getSlot("mod"))

		// inplace binary/logic operations

		setSlot("&=", procedure(v, proxyBlock asm( and	value, v ); ))
		setSlot("|=", procedure(v, proxyBlock asm( or	value, v ); ))
		setSlot("^=", procedure(v, proxyBlock asm( xor	value, v ); ))

/*		setSlot("&", method(v, r := int; proxyBlock asm( mov r, value; and	r, v ); r ))
		setSlot("|", method(v, r := int; proxyBlock asm( mov r, value; or	r, v ); r ))
		setSlot("^", method(v, r := int; proxyBlock asm( mov r, value; xor	r, v ); r ))
	*/

		// binary/logic operations

		setSlot("<<",
			method(v,
				r := int
				v isKindOf(Number) ifTrue(
					proxyBlock asm(
						mov	r, value
						shl	r, v
					)
				) ifFalse(
					recx := vreg32 clone setConcrete(ecx) setNoSpill(true)
					proxyBlock asm(
						mov	r, value
						mov	recx, v
						shl	r, cl
						mov	recx, recx
					)
				)
				r
			)
		)

		setSlot(">>",
			method(v,
				r := int
				v isKindOf(Number) ifTrue(
					proxyBlock asm(
						mov	r, value
						shr	r, v
					)
				) ifFalse(
					recx := vreg32 clone setConcrete(ecx) setNoSpill(true)
					proxyBlock asm(
						mov	r, value
						mov	recx, v
						shr	r, cl
						mov	recx, recx
					)
				)
				r
			)
		)

	/*	setSlot(">>",
			method(v,
				r := int
				recx := vreg32 clone setConcrete(ecx) setNoSpill(true)
				proxyBlock asm(
					mov	r, value
					mov	recx, v
					shr	r, cl
					mov	recx, recx
				)
				r
			)
		)*/

	//	setSlot(">>", method(v, r := int; proxyBlock asm( mov r, value; shr	r, v ); r ))

		setSlot("&", method(v, r := int; proxyBlock asm( mov r, value; and	r, v ); r ))
		setSlot("|", method(v, r := int; proxyBlock asm( mov r, value; or	r, v ); r ))
		setSlot("^", method(v, r := int; proxyBlock asm( mov r, value; xor	r, v ); r ))


		// function call

		callWith := method(

			target := vreg32 clone
			result := vreg32 clone

			lock_eax := vreg32 clone setConcrete(eax) setNoSpill(true)
			lock_ecx := vreg32 clone setConcrete(ecx) setNoSpill(true)

			stackDisp := 0
			call message arguments reverse foreach(argMsg,

			//	argMsg println

				arg := call sender doMessage( argMsg )

	/*			arg println
				arg protos foreach(p, p type println)
				if (arg type == "Reference",
					arg protos at(0) protos foreach(p, p type println)
				)*/

				if( arg isKindOf(Number),
					proxyBlock asm( push arg )						// let's avoid dirty Number's namespace
					stackDisp = stackDisp + 4,
					arg push
					stackDisp = stackDisp + arg pushBytes
				)
			)

			if(value isKindOf(BasicBlock)) then(
				// recursive function ~
				target = owner top
			) else (
				proxyBlock asm( mov target, value )
			)

			proxyBlock asm(
				mov lock_eax, lock_eax	// barriers. removed by loophole optimisation
				mov lock_ecx, lock_ecx
				call target
				add esp, stackDisp
				mov result, lock_eax
				mov lock_ecx, lock_ecx
			)
			setProxyBlock(proxyBlock tail)
			r := int setValue(result)
		//	self println
		//	r println
			r
		)

	)

	// Reference : memory location - implicit load / store

	Reference := Primitive clone do(
		// mixin with type to give stuff
		newSlot("_value")
		newSlot("base")
		newSlot("disp")
		newSlot("getMsg")
		newSlot("putMsg")

	/*	init := method(
			prependProto(Object)
		)*/

		// default : pointer
		with := procedure(str,
			setGetMsg( (str .. "Get") asMessage)
			setPutMsg( (str .. "Put") asMessage)
		)

		// shortcut for affectation
		setSlot("<-",
			method(v,
			//	(_value isNil) ifFalse( Exception raise("Reference <-: overwriting unsynchronised state !") )
			//	msg := message( (base + disp) ) setAttached( putMsg clone appendArg( message(r) setCachedResult(v) ) )
			//	"'#{msg}'" interpolate println
				(base + disp) doMessage( putMsg clone appendArg( message(r) setCachedResult(v) ) )
//			doMessage( msg )
			//	_value = nil
			)
		)

		// sync operation
		setSlot("!",
			method(
				(_value isNil) ifTrue( Exception raise("Reference sync: nothing the synchronize") )
			//	msg := message( (base + disp) ) setAttached( putMsg clone appendArg( message(r) setCachedResult(_value) ) )
			//	"'#{msg}'" interpolate println
			//	doMessage( msg )
				(base + disp) doMessage( putMsg clone appendArg( message(r) setCachedResult(_value) ) )
				_value = nil
			)
		)

		value := method(
			_value isNil ifTrue(
			//	msg := message( (base + disp) ) clone setAttached( getMsg )
			//	"'#{msg}'" interpolate println
		//		self println
			//	t := doMessage( msg )
			//	t := (base + disp) doMessage( getMsg )
			//	t println
			//	"'#{msg}'" interpolate println
				_value = (base + disp) doMessage( getMsg ) value
				name isNil ifFalse( _value setName("*" .. name) )
		//		self println
			)
		//	_value println
			_value
		)

		morphInto := procedure(p,

		//	p type println

		//	self appendProto( p )

		//	protos foreach

			self prependProto( p )
			self setValue( Reference getSlot("value") )
			self setSlot("<-", Reference getSlot("<-") )
		//	self setSlot("!", Reference getSlot("!") )
		//	self removeProto( Reference )
		//	self prependProto( Reference shallowCopy removeProto(Object) )
		)
	)

	ref := method(
		(call argCount == 3) ifTrue(
			r := call sender doMessage(message(Reference clone setProxy)) with("ptr") morphInto(call evalArgAt(2))
		) ifFalse(
			r := call sender doMessage(message(Reference clone setProxy)) with(call evalArgAt(3)) morphInto(call evalArgAt(2))
		)
		r setBase(call evalArgAt(0))
		r setDisp(call evalArgAt(1))

	//	r println
	//	r protos foreach(p,p type println)
		r
	)

	// syntactic sugar :)
	WordPtr := Integer clone do(
		squareBrackets := method(
			// ugly. this is to keep assembler semantics working
			// todo : parse assembly  syntax to give higher priority to x86 symbols

		//	call sender self type println

			call sender self isKindOf(Implementation) ifTrue(
				r := ref(self, call evalArgAt(0) << 2, Integer)
			) ifFalse(
				r := call delegateTo( self protos select(hasSlot("squareBrackets")) at(0) )
			)
			r
		)
	)
	BytePtr := Integer clone do(
		squareBrackets := method(
			// ugly. this is to keep assembler semantics working
			// todo : parse assembly  syntax to give higher priority to x86 symbols

		//	call sender self type println

			call sender self isKindOf(Implementation) ifTrue(
				r := ref(self, call evalArgAt(0), Integer, "uint8")
			) ifFalse(
				r := call delegateTo( self protos select(hasSlot("squareBrackets")) at(0) )
			)
			r
		)
	)
//	Byte := Primitive clone do( squareBrackets := method( BytePtr clone setProxy(call sender) ))

	// C structures
	ByteArray := Integer clone do(
		bytes := method( BytePtr clone ref(self, 0, BytePtr) )
	)

	IoObject := Integer clone do(
	//	dataPtr := method( proxy;  int32atBytes(16) )
	//	dataDouble := method( proxy; int32FromFloat64AtBytes(16) )
	/*	dataPtr := method( Reference clone setProxy with("ptr") setBase(self) setDisp(16) )
		dataDouble := method( Reference clone setProxy with("int32FromFloat64") setBase(self) setDisp(16) )
		state := method( Reference clone setProxy with("ptr") setBase(self) setDisp(32) )
	*/
		data.ptr := method( ref(self, 16, Integer) )
		data.d := method( ref(self, 16, Integer, "int32FromFloat64") )
		state := method( ref(self, 32, IoState) )
	)

	IoState := Integer clone do(

		numberWithDouble := method(d,
			IoState_numberWithDouble_ := int setValue( Linker IoState_numberWithDouble_ )
			IoState_numberWithDouble_ callWith(self, d)
		)

	//	dataPtr := method( proxy;  int32atBytes(16) )
	//	dataDouble := method( proxy; int32FromFloat64AtBytes(16) )
	//	dataPtr := method( Reference clone setProxy with("ptr") setBase(self) setDisp(16) )
	//	dataDouble := method( Reference clone setProxy with("int32FromFloat64") setBase(self) setDisp(16) )
	//	state := method( Reference clone setProxy with("ptr") setBase(self) setDisp(32) )
	)

	Implementation := Object clone do(
		appendProto(CodeGeneration)
		appendProto(Loki)
//		newSlot("imp")
		newSlot("debug", false)
		newSlot("cfg")
		newSlot("targetObject")
		newSlot("targetName")

		with := procedure(
		//	self := self clone

			cfg = CodeGeneration ControlFlowGraph clone
		//	cfg appendProto(Loki) // we are going to evaluate code there ~
		//	target := cfg target // proxy

			proxyBlock := method( cfg target )
			setProxyBlock := method(nlb, cfg setTarget(nlb) )

//			IoState_numberWithDouble_ := Function clone setProxy setPointer( Linker IoState_numberWithDouble_ )
//			IoMessage_locals_valueArgAt_ := Function clone setProxy setPointer( Linker IoMessage_locals_valueArgAt_ )

			IoState_numberWithDouble_ := int setValue( Linker IoState_numberWithDouble_ )
			IoMessage_locals_valueArgAt_ := int setValue( Linker IoMessage_locals_valueArgAt_ )

			valueArgAt := method(i,
			//	target := proxyBlock;
				IoMessage_locals_valueArgAt_ callWith(msg, locals, i) castAs(IoObject)
			)

			_setSlot := getSlot("setSlot")
			_setSlot("setSlot",
			//	method(_setSlot(call evalArgAt(0),call evalArgAt(1)) )
				method(
				//	n := call evalArgAt(0)
				//	x := call evalArgAt(1)
				//	_setSlot(n,x)
				//	_setSlot("n", )
				//	call delegateToMethod()
				//	_setSlot( call evalArgAt(0) , call evalArgAt(1) ) /*setName(n)*/
					call sender _setSlot( call evalArgAt(0) , call evalArgAt(1) ) ?setName(call evalArgAt(0))
				)
			)

			_self := IoObject clone setProxy setValue(cfg refArgAt(0))
			locals := IoObject clone setProxy setValue(cfg refArgAt(1))
			msg := IoObject clone setProxy setValue(cfg refArgAt(2))

		//	_self name println
		//	_self println
		//	_self state println

			doMessage( call argAt(0) )

			_setSlot("setSlot", getSlot("_setSlot"))


		//	doMessage( call message argument at(0) )
		//	call evalArgAt(0)

		//	raLog := cfg registerAllocation

		//	cfg asStringInfos println

		//	rac measure(

			raLog := cfg registerAllocation
			cfg optimise

		//	)

		//	cfg asStringInfos println

			debug ifTrue(raLog println)
		//	cfg printInfos

	//		Sequence nsieve := Linker makeCFunction(Linker hexSeqToBytes(nsieve_cfg encode), "nsieve", Sequence)
	//		imp = Linker makeCFunction(Linker hexSeqToBytes(nsieve_cfg encode), "nsieve", Sequence)

			targetObject isNil ifFalse(link)
		)

		DBG := procedure( setDebug(true) )

	//	impFor := method(object,  Linker makeCFunction(Linker hexSeqToBytes(nsieve_cfg encode), "nsieve", object) )
		link := procedure( targetObject setSlot(targetName, Linker makeCFunction(Linker hexSeqToBytes(cfg encode), targetName, targetObject)) )
		linkToAs := procedure(object, name, targetObject = object; targetName = name; cfg isNil ifFalse(link) )
	)

); // Loki do
