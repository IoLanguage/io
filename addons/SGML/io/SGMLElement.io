Sequence writeToStream := method(b, b appendSeq(self))

SGMLElement := Object clone do(
	//metadoc SGMLElement category Parsers
	//metadoc SGMLElement Representation of an SGML / HTML / XML tag.
	//doc SGMLElement setName(aString) Sets the tag name. Returns self.
	//doc SGMLElement name Returns the tag name
	//doc SGMLElement attributes Returns a Map containing the tag's attributes.
	//doc SGMLElement subitems Returns a List containing the tag's subitems.
	//doc SGMLElement asString Returns a String representation of the tag and all of its subitems.

	name ::= nil
	attributes ::= Map clone
	subitems ::= List clone
	text ::= nil
	parent ::= nil

	attribute := method(aname, attributes at(aname))
	
	withText := method(text,
		self clone setText(text)
	)

	init := method(
		self attributes := attributes clone
		self subitems := subitems clone
	)

	asString := method(
		b := Sequence clone
		writeToStream(b)
		b asSymbol
	)

	writeToStream := method(b, l,
		l = ""
		//if(l == nil, l = "", l = l .. "  ")
		//if(name, b appendSeq(l); beginTag(b), if(text, b appendSeq(l, text, "\n")))
		if(name, b appendSeq(l); beginTag(b), if(text, b appendSeq(l, text)))
		subitems foreach(writeToStream(b, l))
		if(name, b appendSeq(l); endTag(b))
	)

	beginTag := method(b,
		if(b == nil, b := Sequence clone)
		if(name,
			b appendSeq("<", name)
			attributes keys sort foreach(k,
				v := attributes at(k)
				b appendSeq(" ", k, "=\"", v, "\"")
			)
			b appendSeq(">")
		)
		//b appendSeq("\n")
		b
	)

	endTag := method(b,
		if(b == nil, b := Sequence clone)
		b appendSeq("</", name, ">")
		//b appendSeq("\n")
		b
	)

	// extras

	elementWithName := method(name,
		if(name == self name, return self)
		subitems foreach(elementWithName(name) returnIfNonNil)
		nil
	)

	elementsWithName := method(name, list,
		if(list == nil, list := List clone)
		if(name == self name, list append(self))
		subitems foreach(elementsWithName(name, list))
		list
	)
	
	elementsWithNameAndClass := method(name, class,
		elementsWithName(name) select(attribute("class") == class)
	)
	
	elementsWithNameAndClasses := method(
		args := call evalArgs
		name := args removeFirst
		classes := args
		elementsWithName(name) select(e,
			if(classAttribute := e attributes at("class"), classAttribute splitNoEmpties(" ") containsAll(classes))
		)
	)
	
	elementsWithNameAndId := method(name, id,
		elementsWithName(name) select(attributes at("id") == id)
	)
	
	firstText := method(subitems first ?text)

	asObject := method(prefix,
		if(prefix == nil, prefix = "XML")
		if(text, return text)
		if(subitems size == 1 and subitems first text, return subitems first text)

		p := getSlot(prefix .. name ?asCapitalized)
		obj := if(p, p clone, Object clone)

		subitems foreach(v,
			if(v text, continue)
			k := v name
			while(k containsSeq(":"), k = k afterSeq(":"))
			k = k asMutable replaceSeq("?", "")
			plural := k .. "s"
			ev := obj getLocalSlot(k)
			if(ev) then(
				l := obj getLocalSlot(plural)
				if(l == nil, l := obj setSlot(plural, List clone append(ev)))
				l append(v asObject)
			) else(
				obj setSlot(k, v asObject)
			)
		)
		obj
	)

	redirectStrings := method(
		redirects := self elementsWithName("meta")
		redirects selectInPlace(attributes at("content"))
		redirects mapInPlace(attributes at("content") afterSeq("URL="))
		redirects selectInPlace(!= nil)
	)

	linkStrings := method(
		self elementsWithName("a") mapInPlace(attributes at("href"))
	)

	search := method(b, list,
		if(list == nil, list := List clone)
		if(b call(self), list append(self))
		subitems foreach(item, item setParent(self))
		subitems foreach(item, item search(b, list))
		list
	)
	
	tableData := method(
		//elementsWithName("tr") map(search(block(e, e name == "td" or e name == "th")) map(name))
		//elementsWithName("tr") map(subitems map(name))
		elementsWithName("tr") map(subitems map(name))
	)
	
	allText := method(
		if(?text, text, "") .. subitems map(allText) select(!=nil) join
	)
)


SGMLParser do(
	elementProto := SGMLElement

	init := method(
		self stack := List clone
		self root := elementProto clone
		stack push(root)
	)

	top := method(
		if(stack last, stack last, root)
	)

	startElement := method(name,
		e := elementProto clone setName(name asFixedSizeType)
		top subitems append(e)
		e setParent(top)
		stack push(e)
	)

	endElement := method(name,
		stack pop
	)

	newAttribute := method(k, v,
		top attributes atPut(k, v)
	)

	newText := method(text,
		//top subitems append(text)
		top subitems append(elementProto withText(text asFixedSizeType))
	)

	elementForString := method(aString,
		parse(aString)
		root
	)
	
)

Sequence do(
	//doc Sequence asHTML SGML extension to interpret the Sequence as HTML and return an SGML object using SGMLParser elementForString
	asHTML := method(SGMLParser clone elementForString(self asUTF8))
	//doc Sequence asXML SGML extension to interpret the Sequence as XML and return an SGML object using SGMLParser elementForString
	asXML  := method(SGMLParser clone elementForString(self asUTF8))
	//doc Sequence asSGML SGML extension to interpret the Sequence as SGML and return an SGML object using SGMLParser elementForString
	asSGML := method(SGMLParser clone elementForString(self asUTF8))
)

SGML := Object clone do(
	SGMLElement := SGMLElement
	SGMLParser := SGMLParser
)

