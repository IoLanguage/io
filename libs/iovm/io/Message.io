Message do(
	/*doc Message union(other) Creates a union of the receiver and the other parameter. 
	Returns a new message object with the receivers message as the first argument of the returned message, 
	and the other parameter's arguments as each successive argument to the new message.
	*/
	union := method(
		r := Message clone
		l := list(self)
		call message argAt(0) arguments foreach(arg, l append(arg))
		r setArguments(l)
	)

	asSimpleString := method(
		s := self asString asMutable replaceSeq(" ;\n ; ", "")
		if(s size > 40,
			s slice(0, 37) .. "..."
		,
			s
		)
	)
)
