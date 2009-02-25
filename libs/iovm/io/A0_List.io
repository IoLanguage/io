Object do(
  /*doc Object inlineMethod
  Creates a method which is executed directly in a receiver (no Locals object is created).
  <br/>
  <pre>
  Io> m := inlineMethod(x := x*2)
  Io> x := 1
  ==> 1
  Io> m
  ==> 2
  Io> m
  ==> 4
  Io> m
  ==> 8
  </pre>
  */  
	inlineMethod := method(call message argAt(0) setIsActivatable(true))
)

List do(
	unique := method(
		u := List clone
		self foreach(v, u appendIfAbsent(v))
		u
	)

	select := method(
		aList := List clone

		a1 := call argAt(0)
		if(a1 == nil,
			Exception raise("missing argument")
			return
		)
		a2 := call argAt(1)
		a3 := call argAt(2)

		if(a3,
			a1 := a1 name
			a2 := a2 name
			self foreach(i, v,
				call sender setSlot(a1, i)
				call sender setSlot(a2, getSlot("v"))
				ss := stopStatus(c := a3 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				if(getSlot("c"), aList append(getSlot("v")))
			)
			return aList
		)

		if(a2,
			a1 := a1 name
			self foreach(v,
				call sender setSlot(a1, getSlot("v"))
				ss := stopStatus(c := a2 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				if(getSlot("c"), aList append(getSlot("v")))
			)
			return aList
		)

		self foreach(v,
			ss := stopStatus(c := a1 doInContext(getSlot("v"), call sender))
			if(ss isReturn, ss return getSlot("c"))
			if(ss stopLooping, break)
			if(ss isContinue, continue)
			if(getSlot("c"), aList append(getSlot("v")))
		)
		aList
	)

	detect := method(
		a1 := call argAt(0)
		if(a1 == nil, Exception raise("missing argument"))
		a2 := call argAt(1)
		a3 := call argAt(2)

		if(a3,
			a1 := a1 name
			a2 := a2 name
			self foreach(i, v,
				call sender setSlot(a1, i)
				call sender setSlot(a2, getSlot("v"))
				ss := stopStatus(c := a3 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				if(getSlot("c"), return getSlot("v"))
			)
			return nil
		)

		if(a2,
			a1 := a1 name
			self foreach(v,
				call sender setSlot(a1, getSlot("v"))
				ss := stopStatus(c := a2 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				if(getSlot("c"), return getSlot("v"))
			)
			return nil
		)

		self foreach(v,
			ss := stopStatus(c := a1 doInContext(getSlot("v"), call sender))
			if(ss isReturn, ss return getSlot("c"))
			if(ss stopLooping, break)
			if(ss isContinue, continue)
			if(getSlot("c"), return getSlot("v"))
		)
		nil
	)

	map := method(
		aList := List clone

		a1 := call argAt(0)
		if(a1 == nil, Exception raise("missing argument"))
		a2 := call argAt(1)
		a3 := call argAt(2)

		if(a2 == nil,
			self foreach(v,
				ss := stopStatus(c := a1 doInContext(getSlot("v"), call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				aList append(getSlot("c"))
			)
			return aList
		)

		if(a3 == nil,
			a1 := a1 name
			self foreach(v,
				call sender setSlot(a1, getSlot("v"))
				ss := stopStatus(c := a2 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)
				aList append(getSlot("c"))
			)
			return aList
		)

		a1 := a1 name
		a2 := a2 name
		self foreach(i, v,
			call sender setSlot(a1, i)
			call sender setSlot(a2, getSlot("v"))
			ss := stopStatus(c := a3 doInContext(call sender, call sender))
			if(ss isReturn, ss return getSlot("c"))
			if(ss stopLooping, break)
			if(ss isContinue, continue)
			aList append(getSlot("c"))
		)
		return aList
	)

	groupBy := method(
		aMap := Map clone

		a1 := call argAt(0)
		if(a1 == nil, Exception raise("missing argument"))
		a2 := call argAt(1)
		a3 := call argAt(2)

		if(a2 == nil,
			self foreach(v,
				ss := stopStatus(c := a1 doInContext(getSlot("v"), call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)

				key := getSlot("c") asString

				aMap atIfAbsentPut(key, list())
				aMap at(key) append(v)
			)
			return aMap
		)

		if(a3 == nil,
			a1 := a1 name
			self foreach(v,
				call sender setSlot(a1, getSlot("v"))
				ss := stopStatus(c := a2 doInContext(call sender, call sender))
				if(ss isReturn, ss return getSlot("c"))
				if(ss stopLooping, break)
				if(ss isContinue, continue)

				key := getSlot("c") asString

				aMap atIfAbsentPut(key, list())
				aMap at(key) append(v)
			)
			return aMap
		)

		a1 := a1 name
		a2 := a2 name
		self foreach(i, v,
			call sender setSlot(a1, i)
			call sender setSlot(a2, getSlot("v"))
			ss := stopStatus(c := a3 doInContext(call sender, call sender))
			if(ss isReturn, ss return getSlot("c"))
			if(ss stopLooping, break)
			if(ss isContinue, continue)

			key := getSlot("c") asString

			aMap atIfAbsentPut(key, list())
			aMap at(key) append(v)
		)
		return aMap
	)

  //doc List copy(v) Replaces self with <tt>v</tt> list items. Returns self. 
	copy := method(v, self empty; self appendSeq(v); self)

  //doc List mapInPlace Same as <tt>map</tt>, but result replaces self.
	mapInPlace := method(
		self copy(self getSlot("map") performOn(self, call sender, call message))
	)

  //doc List selectInPlace Same as <tt>select</tt>, but result replaces self.
	selectInPlace := method(
		self copy(self getSlot("select") performOn(self, call sender, call message))
	)

	empty := method(self removeAll)

	isEmpty := method(size == 0)
	isNotEmpty := method(size > 0)

	//doc List reverse Reverses the ordering of all the items of the receiver. Returns copy of receiver.
	reverse := method(itemCopy reverseInPlace)

	//doc List itemCopy Returns a new list containing the items from the receiver.
	itemCopy := method(List clone copy(self))

	sort := method(self clone sortInPlace)
	sortBy := method(b, self clone sortInPlaceBy(getSlot("b")))
	/*
	print := method(
		e := try(
			s := Sequence clone
			s appendSeq("list(")
			self foreach(i, v,
				if(i != 0, s appendSeq(", "))
				s appendSeq(getSlot("v") asString)
			)
		)

		if(e,
			s := Sequence clone
			s appendSeq("list(")
			self foreach(i, v,
				if(i != 0, s appendSeq(", "))
				vs := "[exception]"
				try(vs := getSlot("v") asString)
				s appendSeq(vs)
			)

		)
		s appendSeq(")")
		s print
		self
	)
	*/
  //doc List second Returns second element (same as <tt>at(1)</tt>)
	second := method(at(1))
	//doc List second Returns third element (same as <tt>at(2)</tt>)
	third := method(at(2))
)
