CGI := Object clone do(
	docCopyright("Steve Dekorte", 2004)
	docLicense("BSD revised")
	docDescription("
	CGI supports accessing CGI parameters passed in environment variable or standard input by a web servers like Apache.Example use;
	<pre>
	#!./ioServer

	write(\"Content-type:text/html\n\n\")
	write(\"&lt;!doctype html public \"-//W3C/DTD HTML 4.0/EN\"&gt;\n\")
	write(\"Got the following CGI parameters:&lt;PRE&gt;\")

	form = CGI clone parse

	if(form != nil) then(
	  form foreach(k, v, write(\"  \", k, \" = \", v, \"&lt;BR&gt;\"))
	) else(
	  write(\"no form variables found\n\")
	)</pre>")
	docCategory("Networking")

	docSlot("parse", "Parses the QUERY_STRING environment variable (or standard input if there is no QUERY_STRING environment variable) and returns a Map containing key/value query value pairs.")

	isInWebScript := method(
		System getenv("QUERY_STRING") != nil
	)

	newSlot("postData")

	parse := method(
		q := System getenv("QUERY_STRING")
		contentLength := System getenv("CONTENT_LENGTH")
		if (q == nil, return Map clone)

		if (contentLength,
			post := File clone standardInput readStringOfLength(contentLength asNumber)
			q = if (q != "", q .. "&" .. post, post)
		)
		//write("<PRE> q := ", q, "</PRE>\n")
		self parseString(q)
	)

	parseString := method(q,
		q = q asMutable replaceSeq("+", " ")

		form := Map clone
		q splitNoEmpties("&") foreach(i, v,
			kv := v splitNoEmpties("=")
			if(kv size == 2,
				k := kv at(0)
				v := decodeUrlParam(kv at(1))

				if (form hasKey(k),
					vallist := List clone

					// this already exists, so we want to take the value of k,
					// and make a list of the values,
					// remove the original k/v then add k/v as list back.

					if (form at(k) type == "List") then(
						//it's a list already, so just add the value at the end
						vallist = form at(k)
						vallist append(v)
					) else(
						vallist append(form at(k))
						vallist append(v)
					)

					form removeAt(k)
					form atPut(k, vallist)
				) else(
					form atPut(k, v)
				)

			) else(
				kv := kv at(0) splitNoEmpties(",")
				form atPut("imageMapX", kv at(0))
				form atPut("imageMapY", kv at(1))
			)
			if(System getenv("REQUEST_METHOD") asLowercase == "post",
				setPostData(File standardInput open contents)
			)
		)
		return form
	)

	urlCode2Char := Map clone
	urlChar2Code := Map clone
	method(
		for (i, 0, 255,
			hex := i asString toBase(16) asUppercase
			if (hex size == 1, hex = "0" .. hex)
			hex = "%" .. hex

			c := Sequence clone setEncoding("number") setX(i)

			urlChar2Code atPut(c, hex)
			urlCode2Char atPut(hex, c)
		)
		urlChar2Code do(
			atPut("*", "*")
			atPut("-", "-")
			atPut(".", ".")
			for (i, 48, 57, atPut(i asCharacter, i asCharacter))
			for (i, 65, 90, atPut(i asCharacter, i asCharacter))
			atPut("_", "_")
			for (i, 97, 122, atPut(i asCharacter, i asCharacter))
			removeAt("%")
			removeAt(" ")
		)
		urlCode2Char removeAt("%25")
	) call

	docSlot("encodeUrlParam(aString)", "Returns a URL encoded version of aString.")

	encodeUrlParam := method(s,
		s = s asUTF8 asMutable
		s replaceSeq("%", "%25") replaceMap(urlChar2Code) replaceSeq(" ", "+")
	)

	docSlot("decodeUrlParam(aString)", "Returns a URL decoded version of aString.")

	decodeUrlParam := method(s,
		t := s asUTF8 asMutable
		t replaceSeq("+", " ") replaceMap(urlCode2Char) replaceSeq("%25", "%")
	)

)
