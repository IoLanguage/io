# List

A mutable array of values. The first index is 0.

- **category**: Core
- **copyright**: Steve Dekorte 2002
- **license**: BSD revised

## append(anObject1, anObject2, ...)

Appends the arguments to the end of the list. Returns self.

## appendIfAbsent(anObject)

Adds each value not already contained by the receiver. Returns self.

## appendSeq(aList1, aList2, ...)

Add the items in the lists to the receiver. Returns self.

## asEncodedList

Returns a Sequence with an encoding of the list.
Nil, Number and Symbol objects are copied into the encoding, for other
object types, referenceIdForObject(item) will be called to request a
reference id for the object.

Also see: List fromEncodedList.

## asJson

Returns JSON encoded representation of a List.

## asMap

The reverse of Map asList: converts a list of lists (key-value pairs) into
a Map. The first item of each pair list must be a sequence. The second item
is the value.

## asMessage

Converts each element in the list to unnamed messages with their cached result
set to the value of the element (without activating).Returns an unnamed message
whose arguments map 1:1 with the elements (after being converted to messages themselves).

## at(index)

Returns the value at index. Returns Nil if the index is out of bounds.

## atInsert(index, anObject)

Inserts anObject at the index specified by index.
Adds anObject if the index equals the current count of the receiver.
Raises an exception if the index is out of bounds. Returns self.

## atPut(index, anObject)

Replaces the existing value at index with anObject.
Raises an exception if the index is out of bounds. Returns self.

## average

Returns the average of the items.

## capacity

Returns the number of potential elements the receiver can hold before it
needs to grow.

## contains(anObject)

Returns true if the receiver contains anObject, otherwise returns false.

## containsAll(list)

Returns true the target contains all of the items in the argument list.

## containsAny(list)

Returns true the target contains any of the items in the argument list.

## containsIdenticalTo(anObject)

Returns true if the receiver contains a value identical to anObject,
otherwise returns false.

## copy(v)

Replaces self with <tt>v</tt> list items. Returns self.

## cursor

Returns a ListCursor for the receiver.

## detect(optionalIndex, value, message)

Returns the first value for which the message evaluates to a non-nil. Example:
<pre>list(1, 2, 3, 4) detect(i, v, v > 2)
==> 3
list(1, 2, 3, 4) detect(v, v > 2)
==> 3</pre>

## difference(list)

Returns a new list containing items from the target list which aren't in the argument list.

## empty

Removes all items from the receiver.

## first(optionalSize)

Returns the first item or Nil if the list is empty.
If optionalSize is provided, that number of the first items in the list are
returned.

## flatten

Creates a new list, with all contained lists flattened into the new list. For example:
<code>
list(1,2,list(3,4,list(5))) flatten
==> list(1, 2, 3, 4, 5)
</code>

## foreach(optionalIndex, value, message)

Loops over the list values setting the specified index and
value slots and executing the message. Returns the result of the last
execution of the message. Example:
<p>
<pre>
list(1, 2, 3) foreach(i, v, writeln(i, " = ", v))
list(1, 2, 3) foreach(v, writeln(v))</pre>

## fromEncodedList(aSeq)

Returns a List with the decoded Nils, Symbols and Numbers from the input raw
array. For each object reference encounters, objectForReferenceId(id) will
be called to allow the reference to be resolved.

Also see: List asEncodedList.

## groupBy

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

## indexOf(anObject)

Returns the index of the first occurrence of anObject
in the receiver. Returns Nil if the receiver doesn't contain anObject.

## insertAfter(item, afterItem)

Inserts item after first occurrence of afterItem and returns self. If afterItem
is not found, item is appended to the end of the list.

## insertAt(item, index)

Inserts item at the specified index. Raises an exception if the index is
out of bounds. Returns self.

## insertBefore(item, beforeItem)

Inserts item before first occurrence of beforeItem or to the end of the list if
beforeItem is not found. Returns self.

## intersect(list)

Returns a new list containing the common values from the target and argument lists.

## itemCopy

Returns a new list containing the items from the receiver.

## join(optionalSeparator)

Returns a String with the elements of the receiver concatenated into one
String. If optionalSeparator is provided, it is used to separate the
concatenated strings. This operation does not respect string encodings.

## justSerialized(stream)

Writes the receiver's code into the stream.

## last(optionalSize)

Returns the last item or Nil if the list is empty.
If optionalSize is provided, that number of the last items in the list are
returned.

## map

Same as <tt>mapInPlace</tt>, but returns results in a new List.

## mapInPlace(optionalIndex, value, message)

Replaces each item in the receiver with the result of applying a given message
to that item. Example:
<pre>list(1, 5, 7, 2) mapInPlace(i, v, i + v)
==> list(1, 6, 9, 5)
list(1, 5, 7, 2) mapInPlace(v, v + 3)
==> list(4, 8, 10, 5)</pre>

## pop

Returns the last item in the list and removes it
from the receiver. Returns nil if the receiver is empty.

## preallocateToSize(aNumber)

Preallocates array memory to hold aNumber number of items.

## prepend(anObject1, anObject2, ...)

Inserts the values at the beginning of the list. Returns self.

## push(anObject1, anObject2, ...)

Same as add(anObject1, anObject2, ...).

## reduce

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

## remove(anObject, ...)

Removes all occurrences of the arguments from the receiver. Returns self.

## removeAt(index)

Removes the item at the specified index and returns the value removed.
Raises an exception if the index is out of bounds.

## removeFirst

Returns the first item and removes it from the list. nil is returned if the list is empty.

## removeLast

Returns the last item and removes it from the list. nil is returned if the list is empty.

## removeSeq

Removes each of the items from the current list which are contained in the sequence passed in.

## rest

Returns a copy of the list but with the first element removed.

## reverse

Reverses the ordering of all the items of the receiver. Returns copy of receiver.

## reverseForeach(index, value, message)

Same as foreach, but in reverse order.

## reverseInPlace

Reverses the ordering of all the items in the receiver. Returns self.

## reverseReduce

Also known as foldr or inject. Combines values in target starting on the right.
If no initial value is paseed the head of the list is used. See List reverse for examples.

## second

Returns second element (same as <tt>at(1)</tt>)

## select

Same as <tt>selectInPlace</tt>, but result is a new List.

## selectInPlace(optionalIndex, value, message)

Like foreach, but the values for which the result of message is either nil
or false are removed from the List. Example:
<pre>list(1, 5, 7, 2) selectInPlace(i, v, v > 3)
==> 5, 7
list(1, 5, 7, 2) selectInPlace(v, v > 3)
==> 5, 7</pre>

## setSize(newSize)

Sets the size of the receiver by either removing excess items or adding nils
as needed.

## size

Returns the number of items in the receiver.

## slice(startIndex, endIndex, step)

Returns a new string containing the subset of the receiver
from the startIndex to the endIndex. The endIndex argument
is optional. If not given, it is assumed to be the end of the string.
Step argument is also optional and defaults to 1, if not given.
However, since Io supports positional arguments only, you need to
explicitly specify endIndex, if you need a custom step.

## sliceInPlace(startIndex, endIndex, step)

Returns the receiver containing the subset of the
receiver from the startIndex to the endIndex. The endIndex argument
is optional. If not given, it is assumed to be the end of the string.
Step argument is also optional and defaults to 1.

## sort

Returns a new list containing the sorted items from the receiver.

## sortBy(aBlock)

Returns a new list containing the items from the receiver, sorted using aBlock as compare function. Example:
<code>list(1, 3, 2, 4, 0) sortBy(block(a, b, a > b))
==> list(4, 3, 2, 1, 0)</code>

## sortInPlace(optionalExpression)

Sorts the list using the compare method on the items. Returns self.
If an optionalExpression is provided, the sort is done on the result of the
evaluation of the optionalExpression on each value.

## sortInPlaceBy(aBlock)

Sort the list using aBlock as the compare function. Returns self.

## sum

Returns the sum of the items.

## swapIndices(index1, index2)

Exchanges the object at index1 with the object at index2.
Raises an exception if either index is out of bounds. Returns self.

## union(list)

Returns a new list containing items from the target and items which are only in the argument list.

## unique

Returns a new list containing all the values in the target, but no duplicates.

## uniqueCount

Returns a list of list(value, count) for each unique value in self.

## with(anObject, ...)

Returns a new List containing the arguments.

