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
    /*doc List selectInPlace(optionalIndex, value, message)
    Like foreach, but the values for which the result of message is either nil
    or false are removed from the List. Example:
<pre>list(1, 5, 7, 2) selectInPlace(i, v, v > 3)
==> 5, 7
list(1, 5, 7, 2) selectInPlace(v, v > 3)
 ==> 5, 7</pre>
*/
    selectInPlace := method(
        # Creating a context, in which the body would be executed in.
        context := Object clone prependProto(call sender)
        # Note: this is needed for the Object_forwardLocals() method to
        # work correctly. See IoObject.c:870.
        if(call sender hasLocalSlot("self"),
            context setSlot("self", call sender self)
        )
        # Offset, applied to get the real index of the elements being
        # deleted.
        offset := 0
        argCount := call argCount

        if(argCount == 0, Exception raise("missing argument"))
        if(argCount == 1) then(
            body := call argAt(0)
            size repeat(idx,
                if(at(idx - offset) doMessage(body, context) not,
                    removeAt(idx - offset)
                    offset = offset + 1
                )
            )
        ) elseif(argCount == 2) then(
            eName := call argAt(0) name # Element.
            body  := call argAt(1)
            size repeat(idx,
                context setSlot(eName, at(idx - offset))
                if(context doMessage(body) not,
                    removeAt(idx - offset)
                    offset = offset + 1
                )
            )
        ) else(
            iName := call argAt(0) name # Index.
            eName := call argAt(1) name # Element.
            body  := call argAt(2)

            size repeat(idx,
                context setSlot(iName, idx)
                context setSlot(eName, at(idx - offset))
                if(context doMessage(body) not,
                    removeAt(idx - offset)
                    offset = offset + 1
                )
            )
        )
        self
    )

    //doc List select Same as <tt>selectInPlace</tt>, but result is a new List.
    select := method(
        call delegateToMethod(self clone, "selectInPlace")
    )

    /*doc List detect(optionalIndex, value, message)
    Returns the first value for which the message evaluates to a non-nil. Example:
<pre>list(1, 2, 3, 4) detect(i, v, v > 2)
==> 3
list(1, 2, 3, 4) detect(v, v > 2)
==> 3</pre>
*/
    detect := method(
        # Creating a context, in which the body would be executed in.
        context := Object clone prependProto(call sender)
        # Note: this is needed for the Object_forwardLocals() method to
        # work correctly. See IoObject.c:870.
        if(call sender hasLocalSlot("self"),
            context setSlot("self", call sender self)
        )
        argCount := call argCount

        if(argCount == 0, Exception raise("missing argument"))
        if(argCount == 1) then(
            body := call argAt(0)
            self foreach(value,
                if(getSlot("value") doMessage(body, context),
                    return getSlot("value")
                )
            )
        ) elseif(argCount == 2) then(
            eName := call argAt(0) name # Element.
            body  := call argAt(1)
            self foreach(value,
                context setSlot(eName, getSlot("value"))
                if(context doMessage(body), return getSlot("value"))
            )
        ) else(
            iName := call argAt(0) name # Index.
            eName := call argAt(1) name # Element.
            body  := call argAt(2)

            self foreach(idx, value,
                context setSlot(iName, idx)
                context setSlot(eName, getSlot("value"))
                if(context doMessage(body), return getSlot("value"))
            )
        )
        nil # If nothing found, return nil.
    )

    /*doc List mapInPlace(optionalIndex, value, message)
    Replaces each item in the receiver with the result of applying a given message
    to that item. Example:
<pre>list(1, 5, 7, 2) mapInPlace(i, v, i + v)
==> list(1, 6, 9, 5)
list(1, 5, 7, 2) mapInPlace(v, v + 3)
 ==> list(4, 8, 10, 5)</pre>
    */
    mapInPlace := method(
        # Creating a context, in which the body would be executed in.
        context := Object clone prependProto(call sender)
        # Note: this is needed for the Object_forwardLocals() method to
        # work correctly. See IoObject.c:870.
        if(call sender hasLocalSlot("self"),
            context setSlot("self", call sender self)
        )
        argCount := call argCount

        if(argCount == 0, Exception raise("missing argument"))
        if(argCount == 1) then(
            body := call argAt(0)
            self foreach(idx, value,
                atPut(idx, getSlot("value") doMessage(body, context))
            )
        ) elseif(argCount == 2) then(
            eName := call argAt(0) name # Element.
            body  := call argAt(1)
            self foreach(idx, value,
                context setSlot(eName, getSlot("value"))
                atPut(idx, context doMessage(body))
            )
        ) else(
            iName := call argAt(0) name # Index.
            eName := call argAt(1) name # Element.
            body  := call argAt(2)

            self foreach(idx, value,
                context setSlot(iName, idx)
                context setSlot(eName, getSlot("value"))
                atPut(idx, context doMessage(body))
            )
        )
        self
    )

    //doc List map Same as <tt>mapInPlace</tt>, but returns results in a new List.
    map := method(
        call delegateToMethod(self clone, "mapInPlace")
    )

    //doc List copy(v) Replaces self with <tt>v</tt> list items. Returns self.
    copy := method(v, self empty; self appendSeq(v); self)

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
