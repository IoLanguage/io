//metadoc Regex copyright Steve Dekorte 2005, Daniel Rosengren 2007
//metadoc Regex license BSD revised
//metadoc Regex category Parsers
/*metadoc Regex description
<p>The Regex addon adds support for Perl regular expressions
using the <a href=http://www.pcre.org/>PCRE</a> library by Philip Hazel.</p>

<h4>Example 1</h4>
<pre>	
Io> re := "is.*a" asRegex
Io> "This is a test. This is also a test." \
    allMatchesOfRegex("is.*a") replaceAllWith("is not a")
==> "This is not a test. This is not a test.
</pre>

<h4>Example 2</h4>
<pre>	
Io> "11aabb" allMatchesOfRegex("aa*")
==> list("a", "a")

Io> re := "(wom)(bat)" asRegex
Io> "wombats are cuddly" matchesOfRegex(re) replaceAllWith("$2$1!")
==> batwom!s are cuddly
</pre>
*/

Regex do(
	/*doc Regex matchesIn(aString) 
	Returns a RegexMatches object that enumerates the matches of the receiver
	in the given string.
	*/
	matchesIn := method(aString,
		RegexMatches clone setRegex(self) setString(aString)
	)

	/*doc Regex asRegex
	Returns self.
	*/
	asRegex := method(self)
	
	/*doc Regex nameTable
	Returns a list with the name of each capture.
	The first element will always be nil, because it corresponds to the whole match.
	The second element will contain the name of the first capture, or nil if the first
	capture has no name.
	And so on.
	*/
	nameTable := lazySlot(
		names := list setSize(captureCount + 1)
		namedCaptures keys foreach(key,
			names atPut(namedCaptures at(key), key)
		)
		names
	)
	
	/*doc Regex names
	Returns a list of the name of each named capture.
	If there are no named captures, the list will be empty.
	*/
	names := method(
		nameTable select(isNil not)
	)
	

	_flags := method(
		if(pattern isEmpty, return list)

		_flags := list
		if(isCaseless, _flags append("caseless"))
		if(isDotAll, _flags append("dotAll"))
		if(isExtended, _flags append("extended"))
		if(isMultiline, _flags append("multiline"))
		_flags
	)
	
	/*doc Regex asString
	Returns a string containing a textual representation of the receiver.
	*/
	asString := method(
		if(pattern isEmpty, return resend)

		string := "\"#{pattern}\" asRegex" interpolate
		if(_flags isEmpty not,
			string = string .. " " .. _flags join(" ")
		)
		string
	)
)
