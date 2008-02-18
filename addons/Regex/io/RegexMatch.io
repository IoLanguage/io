Range

//metadoc RegexMatch category Parsers
/*metadoc RegexMatch description
Contains the result of a regular expression match operation.
It acts as a read-only list of captured strings.
The first item is the entire matched string.
Each item after that is a captured sub pattern (anything inbetween
parenthesis in the pattern).

<pre>
Io> match := "37signals" findRegex("([0-9]+)([a-z]+)(!!)?")
==> RegexMatch: "37signals" 

# Item 0 is the entire matched string:
Io> match at(0)
==> 37signals

# Item 1 is the second capture:
Io> match at(1)
==> 37

# Item 2 is the first capture:
Io> match at(2)
==> signals

# You can access captures by name:
Io> match at("number")
==> 37
Io> match at("word")
==> signals
</pre>
*/

RegexMatch do(
	/*doc RegexMatch captures
	Returns a list of captured strings. The first element is the whole match.
	*/
	captures := method(
		if(regex isNil, return list)
		captures = ranges map(range,
			if (range, subject slice(range first, range last), nil)
		)
		captures
	)

	/*doc RegexMatch size
	Returns the number of captures.
	*/
	size := method(
		captures size
	)
	
	/*doc RegexMatch sizeInChars
	Returns the length of the match, in characters.
	*/
	sizeInChars := method(
		string size
	)
	
	/*doc RegexMatch slice(startIndex, [endIndex])
	Returns a new list containing the subset of the receiver from the <em>startIndex</em> to the <em>endIndex</em>.
	The <em>endIndex</em> argument is optional. If not given, it is assumed to be the end of the capture list.
	*/
	slice := method(
		call delegateTo(captures)
	)

	/*doc RegexMatch at(indexOrName)
	Returns the capture with the given index or name. <code>at(0)</code> is the entire match.
	*/
	at := method(indexOrName,
		if (index := indexOf(indexOrName), captures at(index), nil)
	)
	
	/*doc RegexMatch names
	Returns a list of the name of each named capture.
	If there are no named captures, the list will be empty.
	*/
	names := method(
		regex ?names
	)

	/*doc RegexMatch indexOf(name)
	Returns the index of the capture with the given name.
	*/
	indexOf := method(name,
		if (name isKindOf(Sequence), regex ?namedCaptures at(name), name)
	)
	
	/*doc RegexMatch nameOf(index)
	Returns the name of the capture with the given index.
	*/
	nameOf := method(index,
		regex ?nameTable at(index)
	)

	/*doc RegexMatch foreach([index], capture, message)
	Loops through the captures, assigns each capture to <em>capture</em>, and evaluates <em>message</em>.
	Returns a list with the result of each evaluation.
	*/

	/*doc RegexMatch map([index], capture, message)
	Like <code>foreach</code>, but the result of each evaluation of <em>message</em> is returned in a list.
	*/

	/*doc RegexMatch select([index], capture, message)
	Like <code>foreach</code>, but the values for which the result of evaluating <em>message</em> are non-nil are returned
	in a list.
	*/
	foreach := map := select := method(
		call delegateTo(captures)
	)


	/*doc RegexMatch string
	Returns the matched string.
	*/
	string := method(captures first)
		
	/*doc RegexMatch range
	Returns the range of the match in the subject.
	*/
	range := method(ranges first)

	/*doc RegexMatch start
	Returns the index into the subject at which the match starts.
	*/
	start := method(if(range, range first, nil))

	/*doc RegexMatch end
	Returns the index into the subject at which the match ends.
	*/
	end := method(if(range, range last, nil))


	/*doc RegexMatch rangeOf(indexOrName)
	Returns the range of the capture with the given index or name.
	*/
	rangeOf := method(indexOrName,
		if (index := indexOf(indexOrName), ranges at(index), nil)
	)

	/*doc RegexMatch startOf(indexOrName) 
	Returns the index into the subject at which the capture with the given index or name starts.
	*/
	startOf := method(indexOrName,
		if(range := rangeOf(indexOrName), range first, nil)
	)

	/*doc RegexMatch endOf(indexOrName)
	Returns the index into the subject at which the capture with the given index or name ends.
	*/
	endOf := method(indexOrName,
		if(range := rangeOf(indexOrName), range last, nil)
	)


	/*doc RegexMatch expandTo(templateString)
	Returns <em>templateString</em> with capture placeholders replaced with what they represent.
	<code>$0</code> is replaced with the whole match, <code>$1</code> is replaced with the first
	sub capture, etc. <code>${name}</code> is replaced with the capture of that name.
	*/
	_ExpansionRegex := Regex with("""\$ (?> (\d+) | \{ (\w+) \})""") extended
	expandTo := method(templateString,
		templateString matchesOfRegex(_ExpansionRegex) replace(m,
			cap := if(m at(1), m at(1) asNumber, m at(2))
			if(string := at(cap), string, "")
		)
	)

	
	/*doc RegexMatch asString
	Returns a string containing a textual representation of the receiver.
	*/
	asString := method(
		if(regex isNil, return resend)
		"RegexMatch: \"#{string}\"" interpolate
	)
)
