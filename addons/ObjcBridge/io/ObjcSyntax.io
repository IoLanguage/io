
return 

// --------------------------------------

/* doc Message end
Returns self if message is the last message in the chain,
otherwise calls end recursively on self's next message.
*/
Message end := method(if(next, next end, self))

/* doc Message insertNext(message)
Inserts message after current message and appends the previous next message
at the end of message.
*/
Message insertNext := method(m,
	if(next, m end setNext(next))
	setNext(m)
)

/* doc Message makeColonUnary
*/
Message makeColonUnary := method(l,
	if(next,
		if(next name == ":",
			arg := next arguments first
			while(arg ?next and arg next ?next, arg := arg next)
			if(arg ?next, next insertNext(arg next); arg setNext(nil))
		)
		next makeColonUnary
	)
	self
)

/* doc Message joinColons
If next message is ":" append the colon to the current message name and
set self's arguments to the colons next message and self's next message
to the message following the ":" arguments.
The setting of the arguments and the next message is also performed if
the current message name ends with ":". However no extra ":" is appended
to the message name in this case.

Example (Note that the ":" are separate messages in this case):

<code>
(NSString stringWithCharacters : "abc" length : 3)
</code>

becomes:

<code>
(NSString stringWithCharacters:("abc") length:(3))
</code>

after applying joinColons to the message representing the above statement.
*/
Message joinColons := method(l,
	args := nil
	if(next and next name == ":",
		self setName(self name .. ":")
		args = next next
	,
		if(self name endsWithSeq(":"),
			args = next
		)
	)
	if(args,
		self setNext(args next)
		args setNext(nil)
		self setArguments(list(args))
	)
	next ?joinColons
	self
)

/* doc Message unInfix
Creates one message out of the infix notation of an Objective-C method.
For example:

<code>
(NSString stringWithCharacters:("abc") length:(3))
</code>

gets transformed into:

<code>
(NSString stringWithCharacters:length:("abc", 3))
</code>
*/
Message unInfix := method(l,
	if(self name endsWithSeq(":"),
		while(next and next name endsWithSeq(":"),
			self setName(self name .. next name)
			self setArguments(self arguments appendSeq(next arguments))
			self setNext(next next)
		)
	)
	next ?unInfix
	self
)

/* doc Object squareBrackets
If squareBrackets are encountered call makeColonUnary, followed by
joinColons followed by unInfix.
The squareBrackets name is also removed to create normal Messages.
Afterwards evaluate the transformed message.
*/
Object squareBrackets := method(
	call message argAt(0) makeColonUnary joinColons unInfix
	call message setName("")
	call sender doMessage(call message argAt(0), call sender)
)

addVariableNamed := method(name,
	self setSlot(name, doString("method(?_" .. name .. ")"))
	self setSlot("set" .. name asCapitalized .. ":", doString("method(value, self _" .. name .. " := value ; self)"))
	nil
)



