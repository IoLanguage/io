# Map

A key/value dictionary appropriate for holding large key/value collections.

- **category**: Core
- **copyright**: Steve Dekorte 2002
- **license**: BSD revised

## asJson

Converts a Map to a string that represents contents in JSON-compilant form

## asList

Converts a Map to a list of lists. Each element in the returned list will be a list of two elements: the key, and the value.

## asObject

Create a new Object whose slotDescriptionMap will be equal to self

## at(keyString, optionalDefaultValue)

Returns the value for the key keyString. Returns nil if the key is absent.

## atIfAbsentPut(keyString, aValue)

If a value is present at the specified key, the value is returned.
Otherwise, inserts/sets aValue and returns aValue.

## atPut(keyString, aValue)

Inserts/sets aValue with the key keyString. Returns self.

## detect(optionalIndex, optionalValue, message)

Returns a random value for which message evals to non-nil.

## empty

Removes all keys from the receiver. Returns self.

## foreach(optionalKey, value, message)

For each key value pair, sets the locals key to
the key and value to the value and executes message.
Example:
<pre>	aMap foreach(k, v, writeln(k, " = ", v))
aMap foreach(v, write(v))</pre>

Example use with a block:

<pre>	myBlock = block(k, v, write(k, " = ", v, "\n"))
aMap foreach(k, v, myBlock(k, v))</pre>

## hasKey(keyString)

Returns true if the key is present or false otherwise.

## hasValue(aValue)

Returns true if the value is one of the Map's values or false otherwise.

## isEmpty

Returns true if this map doesn't contain any pairs.

## isNotEmpty

Returns true if this map contains at least one pair.

## justSerialized(stream)

Writes the receiver's code into the stream.

## keys

Returns a List of the receivers keys.

## map(key, value, message)

Create a List of results of message applied to self's items in a random
order.

## merge(anotherMap)

Returns a new map created by merging the pairs from anotherMap into this map

## mergeInPlace(anotherMap)

Merges the pairs from anotherMap into this map

## removeAt(keyString)

Removes the specified keyString if present. Returns self.

## reverseMap

Creates a new Map using values as keys and keys as values

## select(optionalIndex, optionalValue, message)

Creates a new Map with keys,values of self for which message evaluated
to non-nil.

## size

Returns the number of key/value pairs in the receiver.

## values

Returns a List of the receivers values.

## with(key1,

value1, key2, value2, ...) Returns a new map containing the given keys and values

