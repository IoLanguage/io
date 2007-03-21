Message end := method(if(next, next end, self))

Message insertNext := method(m,
	if(next, m end setNext(next))
	setNext(m)
)

Message makeColonUnary := method(l,
	if(next,
		if(next name == ":",
			arg := next arguments first
			while(arg next and arg next next, arg := arg next)
			if(arg next, next insertNext(arg next); arg setNext(nil))
		)
		next makeColonUnary
	)
	self
)

Message joinColons := method(l,
	if(next,
		if(next name == ":",
			self setName(self name .. ":")
			self setArguments(next arguments)
			self setNext(next next)
		)
		next ?joinColons
	)
	self
)

Message unInfix := method(l,
	if(self name endsWithSeq(":") and next and next name endsWithSeq(":"),
		self setName(self name .. next name)
		self setArguments(self arguments appendSeq(next arguments))
		self setNext(next next)
	)
	next ?unInfix
	self
)

squareBrackets := method(
	call message argAt(0) makeColonUnary joinColons unInfix
	call message setName("")
	call sender doMessage(call message argAt(0), call sender)
)

addVariableNamed: := method(name,
	self setSlot(name, doString("method(?_" .. name .. ")"))
	self setSlot("set" .. name asCapitalized .. ":", doString("method(value, self _" .. name .. " := value ; self)"))
	nil
)

NSMakePoint := method(x, y, Point clone set(x, y))
NSMakeSize := method(w, h, Point clone set(w, h))
NSMakeRect := method(x, y, w, h, Box clone set(NSMakePoint(x, y), NSMakeSize(w, h)))

//AddressBook
if(Addon platform != "darwin",
	ABAddressBook       := method(ObjcBridge classNamed("ADAddressBook"))
	ABGroup             := method(ObjcBridge classNamed("ADGroup"))
	ABMultiValue        := method(ObjcBridge classNamed("ADMultiValue"))
	ABMutableMultiValue := method(ObjcBridge classNamed("ADMutableMultiValue"))
	ABPerson            := method(ObjcBridge classNamed("ADPerson"))
	ABRecord            := method(ObjcBridge classNamed("ADRecord"))
	ABSearchElement     := method(ObjcBridge classNamed("ADSearchElement"))
)

//NSToolbarDisplayMode
NSToolbarDisplayModeDefault      := 0
NSToolbarDisplayModeIconAndLabel := 1
NSToolbarDisplayModeIconOnly     := 2
NSToolbarDisplayModeLabelOnly    := 3

//NSToolbarSizeMode
NSToolbarSizeModeDefault := 0
NSToolbarSizeModeRegular := 1
NSToolbarSizeModeSmall   := 2
