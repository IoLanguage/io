
List do(
    docSlot("sum", "Returns the sum of the items.")
    sum := method(s := 0; self foreach(v, s = s + v); s)

    docSlot("average", "Returns the average of the items.")
    average := method(self sum / self size)

    docSlot("shuffle", "Randomizes the order of the elements in the receiver. Returns self.")
    shuffle := method(for(i, 0, size - 1, swapIndices(i, Random value(i, size) floor)))

    docSlot("anyOne", "Returns a random element of the receiver or nil if the receiver is empty.")
    anyOne := method(at(Random value(0, size) floor))
 
    docSlot("removeFirst", "Returns the first item and removes it from the list. nil is returned if the list is empty.")
    removeFirst := method(if(self size != 0, removeAt(0), nil))

    docSlot("removeLast", "Returns the last item and removes it from the list. nil is returned if the list is empty.")
    removeLast := method(self pop)

    docSlot("removeSeq", "Removes each of the items from the current list which are contained in the sequence passed in.")
    removeSeq := method(seq,
	    seq foreach(x, self remove(x))
	    self
    )

    docSlot("join(optionalString)", "Returns a Sequence of the concatenated items with optionalString between each item or simply the concatenation of the items if no optionalString is supplied.")
    join := method(arg,
        s := if(self first type == "List", List clone, Sequence clone)

		if(arg,
			max := self size - 1
			self foreach(i, v, s appendSeq(v); if(i != max, s appendSeq(arg)))
		,
			self foreach(v, s appendSeq(v))
		)
		
		s
    )

    docSlot("insertAfter(item, afterItem)", "Inserts item after first occurance of afterItem and returns self. If afterItem is not found, item is appended to the end of the list.")
    insertAfter := method(item, afterItem,
       i := self indexOf(afterItem) 
       if(i, self atInsert(i + 1, item), self append(item))
       self
    )

    docSlot("insertBefore(item, beforeItem)", "Inserts item before first occurance of beforeItem or to the end of the list if beforeItem is not found. Returns self.")
    insertBefore := method(item, beforeItem,
       i := self indexOf(beforeItem)
       if(i, self atInsert(i, item), self append(item))
       self
    )

    docSlot("insertAt(item, index)", "Inserts item at the specified index. Raises an exception if the index is out of bounds. Returns self.")
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
     
  docSlot("flatten", """Creates a new list, with all contained lists flattened into the new list. For example:
<pre>
list(1,2,list(3,4,list(5))) flatten
==> list(1, 2, 3, 4, 5)
</pre>
  """)
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

	docSlot("asMessage", "Converts each element in the list to unnamed messages with their cached result set to the value of the element (without activating). Returns an unnamed message whose arguments map 1:1 with the elements (after being converted to messages themselves).")
	asMessage := method(
		m := Message clone
		foreach(elem,
			m setArguments(
				m arguments append(Message clone setCachedResult(getSlot("elem")))
			)
		)
		m
	)
     
	docSlot("select(optionalIndex, value, message)", 
		   "Like foreach, but the values for which the result of 
message are non-nil are returned in a new List. Example:
<pre>list(1, 5, 7, 2) select(i, v, v > 3) print
==> 5, 7

list(1, 5, 7, 2) select(v, v > 3) print
==> 5, 7</pre>")     
     
	docSlot("detect(optionalIndex, value, message)", 
		   "Returns the first value for which the message evaluates to a non-nil. Example:
<pre>list(1, 2, 3, 4) detect(i, v, v > 2)
==> 3

list(1, 2, 3, 4) detect(v, v > 2)
==> 3</pre>")

	docSlot("map(optionalIndex, value, message)", "Same as calling mapInPlace() on a clone of the receiver, but more efficient.")
	
	docSlot("cusor", "Returns a ListCursor for the receiver.")
	cursor := method(ListCursor clone setCollection(self)) 

    docSlot("containsAll(list)", "Returns true the target contains all of the items in the argument list.")
    containsAll := method(c, c detect(i, contains(i) not) == nil) 

    docSlot("containsAny(list)", "Returns true the target contains any of the items in the argument list.")
    containsAny := method(c, c detect(i, contains(i)) != nil) 

    docSlot("intersect(list)", "Returns a new list containing the common values from the target and argument lists.")
    intersect := method(c, c select(i, contains(i)))

    docSlot("difference(list)", "Returns a new list containing items from the target list which aren't in the argument list.")
    difference := method(c, select(i, c contains(i) not))

    docSlot("union(list)", "Returns a new list containing items from the target and items which are only in the argument list.")
    union := method(c, self clone appendSeq(c difference(self)))

    docSlot("unique", "Returns a new list containing all the values in the target, but no duplicates.")
    unique := method(a := list; self foreach(v, a appendIfAbsent(v)); a)

		docSlot("reduce", "Also known as foldl or inject. Combines values in target start on the left. reduce(+) or reduce(x, y, x + y).")
		reduce := method(
			accu := first
			if(call message arguments size == 1,
				args := list(nil)
				meth := call argAt(0) name
				slice(1) foreach(x, accu = accu performWithArgList(meth, args atPut(0, x)))
			,
				aName := call argAt(0) name
				bName := call argAt(1) name
				body := call argAt(2)
				slice(1) foreach(x,
					call sender setSlot(aName, accu)
					call sender setSlot(bName, x)
					accu = call sender doMessage(body, call sender)
				)
			)
			accu
		)

		docSlot("reverseReduce", "Also known as foldr and inject. Combines values in target starting on the right. reverseReduce(+) or reverseReduce(x, y, x + y).")
		reverseReduce := method(
			accu := last
			if(call message arguments size == 1,
				args := list(nil)
				meth := call argAt(0) name
				slice(0, -1) reverseForeach(x, accu = x performWithArgList(meth, args atPut(0, accu)))
			,
				aName := call argAt(0) name
				bName := call argAt(1) name
				body := call argAt(2)
				slice(0, -1) reverseForeach(x,
					call sender setSlot(aName, x)
					call sender setSlot(bName, accu)
					accu = call sender doMessage(body, call sender)
				)
			)
			accu
		)
		
		
		mapFromKey := method(
		  e := call argAt(0)
		  m := Map clone
		  self foreach(v,		      
		      k := getSlot("v") doMessage(e) 
		      l := m at(k) 
		      if(l, l append(getSlot("v")), m atPut(k, list(getSlot("v"))))
		  )
		  m
		)

        docSlot("uniqueCount", "Returns a list of list(value, count) for each unique value in self.")
        uniqueCount := method(self unique map(item, list(item, self select(== item) size)))
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
    r := self slice(0, 30) asString
	//r := self asString
	if(r size > 40,
		r slice(0, 37) .. "..."
	,
		r
	)
)
