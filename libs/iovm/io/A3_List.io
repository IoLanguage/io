
List do(
	//doc List sum Returns the sum of the items.
	sum := method(s := 0; self foreach(v, s = s + v); s)

	//doc List average Returns the average of the items.
	average := method(self sum / self size)

	//doc List shuffleInPlace Randomizes the order of the elements in the receiver. Returns self.
	shuffleInPlace := method(for(i, 0, size - 1, swapIndices(i, Random value(i, size) floor)))
	
	//doc List shuffle Randomizes the ordering of all the items of the receiver. Returns copy of receiver.
	shuffle := method(self itemCopy shuffleInPlace)

	//doc List anyOne Returns a random element of the receiver or nil if the receiver is empty.
	anyOne := method(at(Random value(0, size) floor))

	//doc List removeFirst Returns the first item and removes it from the list. nil is returned if the list is empty.
	removeFirst := method(if(self size != 0, removeAt(0), nil))

	//doc List removeLast Returns the last item and removes it from the list. nil is returned if the list is empty.
	removeLast := method(self pop)

	//doc List removeSeq Removes each of the items from the current list which are contained in the sequence passed in.
	removeSeq := method(seq,
		seq foreach(x, self remove(x))
		self
	)

	/*doc List join(optionalString) 
		Returns a Sequence of the concatenated items with 
		optionalString between each item or simply the concatenation of the items if no optionalString is supplied.
	*/
		
	join2 := method(arg,
		s := if(self first type == "List", List clone, Sequence clone)

		if(arg,
			max := self size - 1
			self foreach(i, v, s appendSeq(v); if(i != max, s appendSeq(arg)))
		,
			self foreach(v, s appendSeq(v))
		)

		s
	)

	/*doc List insertAfter(item, afterItem) 
	Inserts item after first occurance of afterItem and returns self. 
	If afterItem is not found, item is appended to the end of the list.
	*/
	insertAfter := method(item, afterItem,
		i := self indexOf(afterItem)
		if(i, self atInsert(i + 1, item), self append(item))
		self
	)

	//doc List insertBefore(item, beforeItem) Inserts item before first occurance of beforeItem or to the end of the list if beforeItem is not found. Returns self.
	insertBefore := method(item, beforeItem,
		i := self indexOf(beforeItem)
		if(i, self atInsert(i, item), self append(item))
		self
	)

	//doc List insertAt(item, index) Inserts item at the specified index. Raises an exception if the index is out of bounds. Returns self.
	insertAt := method(item, index, self atInsert(index, item))

	asString := method(
		s := "list(" asMutable
		self foreach(i, v,
			s appendSeq(getSlot("v") asSimpleString)
			if (i != self size - 1, s appendSeq(", "))
		)
		s appendSeq(")")
	)

	max := method(
		m := call argAt(0)
		obj := self first
		self foreach(o,
			v1 := if(m, o doMessage(m, call sender), o)
			v2 := if(m, obj doMessage(m, call sender), obj)
			if(v1 > v2, obj := o)
		)
		obj
	)

	min := method(
		m := call argAt(0)
		obj := self first
		self foreach(o,
			v1 := if(m, o doMessage(m, call sender), o)
			v2 := if(m, obj doMessage(m, call sender), obj)
			if(v1 < v2, obj := o)
		)
		obj
	)

	/*doc List flatten 
	Creates a new list, with all contained lists flattened into the new list. For example:
<code>
list(1,2,list(3,4,list(5))) flatten
==> list(1, 2, 3, 4, 5)
</code>
	*/
	
	List flatten := method(
		l := List clone
		self foreach(v,
			if(getSlot("v") type == "List",
				l appendSeq(getSlot("v") flatten)
			,
				l append(getSlot("v")))
		)
		l
	)

	/*doc List asMessage Converts each element in the list to unnamed messages 
	with their cached result set to the value of the element (without activating). 
	Returns an unnamed message whose arguments map 1:1 with the elements (after being converted to messages themselves).
	*/
	
	asMessage := method(
		m := Message clone
		foreach(elem,
			m setArguments(
				m arguments append(Message clone setCachedResult(getSlot("elem")))
			)
		)
		m
	)

	/*doc List select(optionalIndex, value, message)",
		"Like foreach, but the values for which the result of
message are non-nil are returned in a new List. Example:
<code>list(1, 5, 7, 2) select(i, v, v > 3) print
==> 5, 7

list(1, 5, 7, 2) select(v, v > 3) print
==> 5, 7</code>
*/

	/*doc List detect(optionalIndex, value, message)
	Returns the first value for which the message evaluates to a non-nil. Example:
<code>list(1, 2, 3, 4) detect(i, v, v > 2)
==> 3

list(1, 2, 3, 4) detect(v, v > 2)
==> 3</code>
*/

	//doc List map(optionalIndex, value, message) Same as calling mapInPlace() on a clone of the receiver, but more efficient.

	//doc List cursor Returns a ListCursor for the receiver.
	cursor := method(ListCursor clone setCollection(self))

	//doc List containsAll(list) Returns true the target contains all of the items in the argument list.
	containsAll := method(c, c detect(i, contains(i) not) == nil)

	//doc List containsAny(list) Returns true the target contains any of the items in the argument list.
	containsAny := method(c, c detect(i, contains(i)) != nil)

	//doc List intersect(list) Returns a new list containing the common values from the target and argument lists.
	intersect := method(c, c select(i, contains(i)))

	//doc List difference(list) Returns a new list containing items from the target list which aren't in the argument list.
	difference := method(c, select(i, c contains(i) not))

	//doc List union(list) Returns a new list containing items from the target and items which are only in the argument list.
	union := method(c, self clone appendSeq(c difference(self)))

	//doc List unique Returns a new list containing all the values in the target, but no duplicates.
	unique := method(a := list; self foreach(v, a appendIfAbsent(v)); a)

	//doc List asMap The reverse of Map asList: converts a list of lists (key-value pairs) into a Map. The first item of each pair list must be a sequence. The second item is the value.
	asMap := method(
		m := Map clone
		self foreach(pair, m atPut(pair at(0), pair at(1)))
	)

		//doc List reduce Also known as foldl or inject. Combines values in target start on the left. reduce(+) or reduce(x, y, x + y).
		reduce := method(
			accu := first
			if(call message arguments size == 1,
				args := list(nil)
				meth := call argAt(0) name
				exSlice(1) foreach(x, accu = accu performWithArgList(meth, args atPut(0, x)))
			,
				aName := call argAt(0) name
				bName := call argAt(1) name
				body := call argAt(2)
				exSlice(1) foreach(x,
					call sender setSlot(aName, accu)
					call sender setSlot(bName, x)
					accu = call sender doMessage(body, call sender)
				)
			)
			accu
		)

		//doc List reverseReduce Also known as foldr and inject. Combines values in target starting on the right. reverseReduce(+) or reverseReduce(x, y, x + y).
		reverseReduce := method(
			accu := last
			if(call message arguments size == 1,
				args := list(nil)
				meth := call argAt(0) name
				exSlice(0, -1) reverseForeach(x, accu = x performWithArgList(meth, args atPut(0, accu)))
			,
				aName := call argAt(0) name
				bName := call argAt(1) name
				body := call argAt(2)
				exSlice(0, -1) reverseForeach(x,
					call sender setSlot(aName, x)
					call sender setSlot(bName, accu)
					accu = call sender doMessage(body, call sender)
				)
			)
			accu
		)


		mapFromKey := method(key,
			e := key asMessage
			m := Map clone
			self foreach(v,
				k := getSlot("v") doMessage(e)
				l := m at(k)
				if(l, l append(getSlot("v")), m atPut(k, list(getSlot("v"))))
			)
			m
		)

		//doc List uniqueCount Returns a list of list(value, count) for each unique value in self.
		uniqueCount := method(self unique map(item, list(item, self select(== item) size)))
		
		exSlice := getSlot("slice")
		
		
	asJson := method(
		"[" .. self map(asJson) join(",") .. "]"
	)
)

List ListCursor := Object clone do(
	newSlot("index", 0)
	newSlot("collection")
	next := method(index = index + 1; max := (collection size - 1); if(index > max, index = max; false, true))
	previous := method(index = index - 1; if(index < 0, index = 0; false, true))
	value := method(collection at(index))
	insert := method(v, collection atInsert(index, getSlot("v")))
	remove := method(v, collection removeAt(index))
)

List asSimpleString := method(
	r := self exSlice(0, 30) asString
	//r := self asString
	if(r size > 40,
		r exSlice(0, 37) .. "..."
	,
		r
	)
)
