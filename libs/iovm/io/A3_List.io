List do(
    //doc List sum Returns the sum of the items.
    sum := method(self reduce(+))

    //doc List average Returns the average of the items.
    average := method(self sum / self size)

    //doc List removeFirst Returns the first item and removes it from the list. nil is returned if the list is empty.
    removeFirst := method(if(self size != 0, removeAt(0), nil))

    //doc List removeLast Returns the last item and removes it from the list. nil is returned if the list is empty.
    removeLast := method(self pop)

    //doc List removeSeq Removes each of the items from the current list which are contained in the sequence passed in.
    removeSeq := method(seq,
        seq foreach(x, self remove(x))
        self
    )

    //doc List rest Returns a copy of the list but with the first element removed.
    rest := method(slice(1))

    /*doc List join(optionalSeparator)
    Returns a Sequence of the concatenated items with
    optionalSeparator between each item or simply the concatenation of the items if no optionalSeparator is supplied.
    */
    join := method(sep,
        result := Sequence clone
        if(sep,
            max := self size - 1
            self foreach(idx, value,
                result appendSeq(value)
                if(idx != max, result appendSeq(sep))
            )
        ,
            self foreach(value, result appendSeq(value))
        )
    result)

    /*doc List insertAfter(item, afterItem)
    Inserts item after first occurrence of afterItem and returns self. If afterItem
    is not found, item is appended to the end of the list.
    */
    insertAfter := method(item, afterItem,
        i := self indexOf(afterItem)
        if(i, self atInsert(i + 1, item), self append(item))
        self
    )

    /*doc List insertBefore(item, beforeItem)
    Inserts item before first occurrence of beforeItem or to the end of the list if
    beforeItem is not found. Returns self.
    */
    insertBefore := method(item, beforeItem,
        i := self indexOf(beforeItem)
        if(i, self atInsert(i, item), self append(item))
        self
    )

    /*doc List insertAt(item, index)
    Inserts item at the specified index. Raises an exception if the index is
    out of bounds. Returns self.
    */
    insertAt := method(item, index, self atInsert(index, item))

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
            if(getSlot("v") isKindOf(List),
                l appendSeq(getSlot("v") flatten)
            ,
                l append(getSlot("v")))
        )
        l
    )

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

    /*doc List reduce
    Also known as foldl or inject. Combines values in target starting on the left.
    If no initial value is passed the head of the list is used. <br />
<pre>
Io> list(1, 2, 3) reduce(+)
==> 6
Io> list(1, 2, 3) reduce(xs, x, xs + x)
==> 6
Io> list(1, 2, 3) reduce(+, -6) # Passing the initial value.
==> 0
Io> list(1, 2, 3) reduce(xs, x, xs + x, -6)
==> 0
</pre>
*/
    reduce := method(
        argCount := call argCount

        if(argCount == 0, Exception raise("missing argument"))
        # Checking for the initial value, if it's not present, the
        # head of the list is used.
        if(argCount == 2 or argCount == 4,
            target := self
            accumulator := call sender doMessage(
                call argAt(argCount - 1)
            )
        ,
            target := slice(1)
            accumulator := first
        )

        if(argCount <= 2,
            args := list(nil)
            method := call argAt(0) name
            target foreach(x,
                accumulator = accumulator performWithArgList(
                    method, args atPut(0, x)
                )
            )
        ,
            aName := call argAt(0) name # Accumulator.
            bName := call argAt(1) name # Item.
            body := call argAt(2)
            # Creating a context, in which the body would be executed in.
            context := Object clone prependProto(call sender)
            # Note: this is needed for the Object_forwardLocals() method to
            # work correctly. See IoObject.c:870.
            if(call sender hasLocalSlot("self"),
                context setSlot("self", call sender self)
            )
            target foreach(x,
                context setSlot(aName, accumulator)
                context setSlot(bName, x)
                accumulator = context doMessage(body)
            )
        )
        accumulator
    )

    /*doc List reverseReduce
    Also known as foldr or inject. Combines values in target starting on the right.
    If no initial value is paseed the head of the list is used. See List reverse for examples.
    */
    reverseReduce := method(
        # An alternative approach is to create a generic method
        # which will take an extra argument formeth, and two
        # wrappers passing foreach and foreachReversed to the
        # generic method.
        call delegateToMethod(self reverse, "reduce")
    )

    //doc List uniqueCount Returns a list of list(value, count) for each unique value in self.
    uniqueCount := method(self unique map(item, list(item, self select(== item) size)))

    exSlice := getSlot("slice")

/*doc List groupBy
    Group items in a List by common expression value and return them aggregated in a Map.
    <em>Note</em>: asJson is used because Map doesn't have asString method implemented.
<pre>
Io> list("a", "b", "cd") groupBy(size) asJson
==> {"2":["cd"],"1":["a","b"]}
Io> list("a", "b", "cd") groupBy(v, v containsSeq("c")) asJson
==> {"false":["a","b"],"true":["cd"]}
Io> list("a", "b", "cd") groupBy(i, v, i == 1) asJson
==> {"false":["a","cd"],"true":["b"]}
</pre>
*/
    groupBy := method(
        result  := Map clone
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
                key := getSlot("value") doMessage(body, context) asString
                result atIfAbsentPut(key, list())
                result at(key) append(getSlot("value"))
            )
        ) elseif(argCount == 2) then(
            eName := call argAt(0) name # Element.
            body  := call argAt(1)
            self foreach(value,
                context setSlot(eName, getSlot("value"))

                key := context doMessage(body) asString
                result atIfAbsentPut(key, list())
                result at(key) append(getSlot("value"))
            )
        ) else(
            iName := call argAt(0) name # Index.
            eName := call argAt(1) name # Element.
            body  := call argAt(2)

            self foreach(idx, value,
                context setSlot(iName, idx)
                context setSlot(eName, getSlot("value"))

                key := context doMessage(body) asString
                result atIfAbsentPut(key, list())
                result at(key) append(getSlot("value"))
            )
        )

        result
    )

    /*doc List asMessage
    Converts each element in the list to unnamed messages with their cached result
    set to the value of the element (without activating).Returns an unnamed message
    whose arguments map 1:1 with the elements (after being converted to messages themselves).
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

    asString := method("list(" .. self join(", ") .. ")")
    asSimpleString := method(
        result := self slice(0, 30) map(asSimpleString) asString
        if(result size > 40,
            result exSlice(0, 37) .. "..."
        ,
            result
        )
    )

    //doc List asJson Returns JSON encoded representation of a List.
    asJson := method(
        "[" .. self map(asJson) join(",") .. "]"
    )

    /*doc List asMap
    The reverse of Map asList: converts a list of lists (key-value pairs) into
    a Map. The first item of each pair list must be a sequence. The second item
    is the value.
    */
    asMap := method(
        m := Map clone
        self foreach(pair, m atPut(pair at(0), pair at(1)))
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

# IMPORTANT:
# ----------
# The following methods be removed, because they aren't:
#   * documented,
#   * unittested,
#   * used anywhere else in the code.
#
# Note: probably, List second() and List third() should also be considered
# for removal.

List do(
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
)
