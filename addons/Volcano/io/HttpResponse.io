Header := Object clone do(
	name ::= nil
	value ::= nil
)

HttpResponse := Object clone do(
	socket ::= nil
	headers ::= nil
	statusCode ::= 200
	contentType ::= "text/html"
	body ::= nil
	
	init := method(
		setHeaders(List clone)
		setBody(Sequence clone)
	)
	
	withSocket := method(aSocket,
		HttpResponse clone setSocket(aSocket)
	)
	
	status := method(
		statusCode asString .. " " .. StatusCodes at(statusCode asString)
	)
	
	addHeader := method(name, value,
		headers append(Header clone setName(name) setValue(value))
		self
	)
	
	setHeader := method(name, value,
		if(header := headers detect(h, h name == name),
			header setName(name) setValue(value)
		,
			addHeader(name, value)
		)
		self
	)
	
	addHeaderIfAbsent := method(name, value,
		headers detect(h, h name == name) ifNil(addHeader(name, value))
		self
	)
	
	send := method(
		socket write("HTTP/1.1 " .. status .. "\r\n")
		addHeaderIfAbsent("Content-Type", contentType)
		headers foreach(header, socket write(header name .. ": " .. header value .. "\r\n"))
		socket write("\r\n")
		socket write(body)
	)
	
	setCookie := method(name, value, expiresDate, domain, path, secureBool,
		if (name == nil, Error with("Can't set cookie with nil name."))
		name = CGI encodeUrlParam(name)
		value = if(value, CGI encodeUrlParam(value), "")
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
		addHeader("Set-Cookie", string)
		self
	)
	
	redirectTo := method(url,
		addHeader("Location", url)
		setStatusCode(302)
		self
	)
)