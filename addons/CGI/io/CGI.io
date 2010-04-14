CGI := Object clone do(
//metadoc CGI Steve Dekorte, 2004
//metadoc CGI license BSD revised
//metadoc CGI category Networking

/*metadoc CGI description
CGI supports accessing CGI parameters passed in environment variables or standard input by a web servers like Apache. Example use:
<pre>
#!./ioServer

cgi = CGI clone

redirect = cgi getParameters at("redirurl")
if (redirect and redirect != "",
	redirect clipAfterStartOfSeq("\r")
	redirect clipAfterStartOfSeq("\n")
	cgi redirect(redirect)
	System exit(0)
 )

cgi header("Content-type", "text/html")

cgi write("&lt;html&gt;&lt;head&gt;&lt;title&gt;test&lt;/title&gt;&lt;body&gt;")
cgi write("GET Parameters:")
cgi getParameters foreach(k, v,
	cgi write(k .. " = " .. v .. ","))
)

cgi write("POST Parameters:")
cgi postParameters foreach(k, v,
	cgi write(k .. " = " .. v .. ","))
)

cgi write("COOKIES:")
cgi cookies foreach(k, v,
	cgi write(k .. " = " .. v .. ",")
)
</pre>
*/

	//doc CGI isInWebScript Checks to see if this is being called within a CGI request or from the command-line (testing).  Simply checks for System getEnvironmentVariable("GATEWAY_INTERFACE")
	isInWebScript := method(
		System getEnvironmentVariable("GATEWAY_INTERFACE") != nil
	)


	//doc CGI maxPostSize Maximum size in bytes, to process from user submitted data.  Data greater than this will result in a nil postData slot
	newSlot("maxPostSize")


	//doc CGI postData The raw post data sent to the script.  Only set if getEnvironmentVariable("REQUEST_METHOD") asLowercase == "post".
	_memoized := Map clone
	postData := method(
		if (_memoized hasKey("postData"), return _memoized at("postData"))

		data := nil
		if(requestMethod == "POST",
			sz := System getEnvironmentVariable("CONTENT_LENGTH") ?asNumber
			if(maxPostSize ?asNumber ?isNan or sz < maxPostSize,
				data = (File standardInput open contents)
			)
		)

		_memoized atPut("postData", data)
		return _memoized at("postData")
	)


	//doc CGI maxPostSizeExceeded Returns true if the POST data exceeds a set maxPostSize
	maxPostSizeExceeded := method(
			sz := System getEnvironmentVariable("CONTENT_LENGTH") ?asNumber
			if(maxPostSize ?asNumber ?isNan or sz < maxPostSize,
				return false,
				return true
			)
	)


	//doc CGI getParameters Parses the QUERY_STRING environment variable and returns a Map containing key/value query value pairs.  For testing, a QUERY_STRING can be passed to standard in, one line will be read
	parse := method(getParameters) // for compatibility
	
	getParameters := method(
		if (_memoized hasKey("getParameters"), return _memoized at("getParameters"))
		if (isInWebScript,
			q := queryString,
			q := File standardInput readLine
		)

		if (q == nil, return Map clone)
		_memoized atPut("getParameters", self parseString(q))
		return _memoized at("getParameters")
	)

/*doc CGI postParameters 
Parses the POST data, multipart and urlencoded.  Returns a map of submitted variables.
For uploaded files, an Object is returned with these slots:
<pre>
fileName
content (raw content of file as Sequence)
contentType
contentEncoding
size (in characters/bytes)
asString (pretty string of name, type, size)
</pre>
*/

	postParameters := method(
		if (_memoized hasKey("postParameters"), return _memoized at("postParameters"))
		form := Map clone
		if(contentType == "application/x-www-form-urlencoded") then(
			form = self parseString(postData)

		) elseif(contentType ?beginsWithSeq("multipart/form-data") and postData != nil)  then(
			boundary := contentType asMutable clipBeforeEndOfSeq("boundary=")
			postData beforeSeq("--" .. boundary .. "--\r\n") splitNoEmpties("--" .. boundary .. "\r\n") foreach(field,
				if (field == "--", continue)
				header := field beforeSeq("\r\n\r\n")
				headers := Map clone
				header splitNoEmpties("\r\n") foreach(h,
					headers atPut(h beforeSeq(":") strip asLowercase, h afterSeq(":") strip)
				)

				body := field afterSeq("\r\n\r\n") asMutable removeSuffix("\r\n")

				disp := headers at("content-disposition")
				subheaders := Map clone
				if (disp,
					disp split(";") foreach(p,
						kv := p split("=")
						subheaders atPut(kv at(0) strip asLowercase, kv at(1) ?strip("\""))
					)
				 )

				file := if(subheaders hasKey("filename") and subheaders at("filename") != "" and body size > 0,
					file := Object clone
					file fileName := subheaders at("filename")
					file content := body
					file contentType := headers at("content-type")
					file contentEncoding := headers at("content-transfer-encoding")
					file size := method(content size)
					file asString := "File: " .. file fileName .. " (".. file contentType .. ") [" .. file size .. " bytes]",
					nil
				)

			)
		)
		_memoized atPut("postParameters", form)
		return _memoized at("postParameters")
	)


	//doc CGI cookies Returns a Map of cookies provided by the client

	cookies := method(
		raw := httpHeader("cookie")
		parsed := Map clone

		raw ?splitNoEmpties(";") foreach(cook,
			cook strip
			parsed atPut(cook beforeSeq("=") strip, decodeUrlParam(cook afterSeq("=") strip))
		)

		return parsed
	)


	/*doc CGI setCookie(name, value, expiresDate, domain, path, secureBool) 
	Sets a cookie, keep in mind this will not be available in cookies() until they next visit to the site.  
	Parameters other than name and value are optional.
	*/

	setCookie := method(name, value, expiresDate, domain, path, secureBool,
		if (name == nil, return false)
		name = encodeUrlParam(name)
		value = if(value, encodeUrlParam(value), "")
		expires := if(expiresDate isKindOf(Date),
			expiresDate -= Duration fromNumber(expiresDate gmtOffsetSeconds)
			"expires=" .. expiresDate asString("%a, %d-%b-%Y %H:%M:%S GMT") .. "; ",
			""
		)

		domain := if(domain,
			"domain=" .. domain .. "; ",
			""
		)

		path := if(path,
			"path=" .. path .. "; ",
			""
		)

		secure := if(secureBool, "secure" , "")

		string := name .. "=" .. value .. "; " .. expires .. domain .. path .. secure
		header("Set-Cookie", string, true)
		return string
	)

	//doc CGI requestParameter(name) Lazy developer's helper funtion. Retrieves a value from GET or POST, POST first

	requestParameter := method(name,
		if(postParameters hasKey(name),
			postParameters at(name),
			getParameters at(name)
		)
	)



	//doc CGI queryString QUERY_STRING from web server
	queryString := method( return System getEnvironmentVariable("QUERY_STRING"))

	//doc CGI pathInfo PATH_INFO from web server
	pathInfo := method( return System getEnvironmentVariable("PATH_INFO"))

	//doc CGI pathTranslated PATH_TRANSLATED from web server
	pathTranslated := method( return System getEnvironmentVariable("PATH_TRANSLATED"))

	//doc CGI scriptName SCRIPT_NAME from web server
	scriptName := method( return System getEnvironmentVariable("SCRIPT_NAME"))

	//doc CGI contentType CONTENT_TYPE from web server
	contentType := method( return System getEnvironmentVariable("CONTENT_TYPE"))

	//doc CGI remoteHost REMOTE_HOST from web server - User's host (often blank)
	remoteHost := method( return System getEnvironmentVariable("REMOTE_HOST"))

	//doc CGI remoteAddress REMOTE_ADDR from web server - User's IP
	remoteAddress := method( return System getEnvironmentVariable("REMOTE_ADDR"))

	//doc CGI contentLength CONTENT_LENGTH from web server - Size of POST Data
	contentLength := method( return System getEnvironmentVariable("CONTENT_LENGTH") ?asNumber)

	//doc CGI httpHeader(name) Fetch a header supplied by the client, such as 'referer'
	httpHeader := method(name, return System getEnvironmentVariable("HTTP_" .. name asUppercase))

	//doc CGI requestMethod GET, POST, PUT, etc
	requestMethod := method(
		m := System getEnvironmentVariable("REQUEST_METHOD") ?asUppercase
		if(m == nil or m == "", m = "GET")
		return m
	)

	//doc CGI encodeUrlParam(aString) Returns a URL encoded version of aString.
	encodeUrlParam := method(s,
		s = s asUTF8 asMutable
		s replaceSeq("%", "%25") replaceMap(urlChar2Code) replaceSeq(" ", "+")
	)

	//doc CGI decodeUrlParam(aString) Returns a URL decoded version of aString.
	decodeUrlParam := method(s,
		t := s asUTF8 asMutable
		t replaceSeq("+", " ") replaceMap(urlCode2Char) replaceSeq("%25", "%")
	)


	sendHeaders := method(
		_headersToSend foreach (k, v,
			if(v isKindOf(List),
				v foreach(v, File standardOutput write(k .. ": " .. v .. "\n")),
				File standardOutput write(k .. ": " .. v .. "\n")
			)
		)
		File standardOutput write("\n")
		_headersSent = true
	)


	//doc CGI write(string, [string...]) Send content for the body of the response
	write := method(
		if (_headersSent == false,
			sendHeaders
		)
		call message argsEvaluatedIn(call sender) foreach(string, File standardOutput write(string))
	)
	_headersSent := false



	/*doc CGI header(name, value, sendMultiple) 
	Add a header to the output, may only be called before write() is called.  
	One of each header will be sent unless sendMultiple is true
	*/
	header := method(name, value, sendMultiple,
		if(_headersSent, Exception raise("Content already sent, no more headers may be transmitted"))
		if(sendMultiple,
			mapPutOrAppendAsList(_headersToSend, name asLowercase, value),
			_headersToSend atPut(name asLowercase, value)
		 )
		return true
	)
	_headersToSend := Map clone atPut("content-type", "text/html")


	//doc CGI redirect(url) Send a location: and redirect the user.  May only be called before write() is called.  It is left to the caller to stop any further processing.
	redirect := method(url,
		if(_headersSent, Exception raise("Content already sent, too late to redirect"))

		File standardOutput write("Location: " .. url ..  "\n\n")
	)


	/*doc CGI status(statusCode) 
	Numeric status code to send to the client.  
	Normally, the server will figure this out on its own, but this allows handling 404s and such.
	*/

	status := method(status,
		header("Status", status ?asNumber)
	)


	newLinesToBr := method(str,
		return str asMutable replaceSeq("\n", "<br />")
	)


	mapPutOrAppendAsList := method(map, at, val,
		if(map hasKey(at) == false) then(
			map atPut(at, val)
		) elseif(map at(at) isKindOf(List)) then(
			list := map at(at) append(val)
		) else(
			list := List clone
			list append(map at(at))
			list append(val)
			map removeAt(at)
			map atPut(at, list)
		)
	)


	parseString := method(q,
		if(q == nil, return Map clone)
		q = q asMutable replaceSeq("+", " ")

		form := Map clone
		q splitNoEmpties("&") foreach(i, v,
			kv := v splitNoEmpties("=")
			if(kv size == 2,
				k := kv at(0)
				v := decodeUrlParam(kv at(1))
				mapPutOrAppendAsList(form, k, v)
			) else(
				ikv := kv at(0) splitNoEmpties(",")
				validx := ikv at(0) ?asNumber isNan == false
				validy := ikv at(1) ?asNumber isNan == false
				if (ikv size == 2 and validx and validy,
					form atPut("imageMapX", ikv at(0))
					form atPut("imageMapY", ikv at(1)),
					form atPut(kv at(0), nil)
				)
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
)
