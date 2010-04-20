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

    /*doc List select(optionalIndex, value, message)
    Like foreach, but the values for which the result of message are non-nil are returned
    in a new List. Example:
<code>list(1, 5, 7, 2) select(i, v, v > 3) print
==> 5, 7
list(1, 5, 7, 2) select(v, v > 3) print
 ==> 5, 7</code>
*/
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

    /*doc List detect(optionalIndex, value, message)
    Returns the first value for which the message evaluates to a non-nil. Example:
<code>list(1, 2, 3, 4) detect(i, v, v > 2)
==> 3
list(1, 2, 3, 4) detect(v, v > 2)
==> 3</code>
*/
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

    //doc List map(optionalIndex, value, message) Same as calling mapInPlace() on a clone of the receiver, but more efficient.
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

    //doc List sort Returns a new list containing the sorted items from the receiver.
    sort := method(self clone sortInPlace)
    /*doc List sortBy(aBlock)
        Returns a new list containing the items from the receiver, sorted using aBlock as compare function. Example:
<code>list(1, 3, 2, 4, 0) sortBy(block(a, b, a > b))
==> list(4, 3, 2, 1, 0)</code>
*/
    sortBy := method(b, self clone sortInPlaceBy(getSlot("b")))

    //doc List second Returns second element (same as <tt>at(1)</tt>)
    second := method(at(1))
    //doc List second Returns third element (same as <tt>at(2)</tt>)
    third := method(at(2))
)
