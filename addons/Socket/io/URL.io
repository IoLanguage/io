
URL := Notifier clone do(
	standardHeaderSymbols := list(
		"Accept-Ranges", "Cache-Control", "Last-Modified", "\r"
	)
	cacheFolder := Directory with("/tmp/ioUrlCache/")
	cacheFolder create
	cacheFile := method(MD5; File with(Path with(cacheFolder path, url md5String)))
	cacheOn ::= false
	cacheTimeout := 24*60*60
	followRedirects ::= true
	maxRedirects ::= 1
	redirectUrlNormalizer ::= nil
	
	cacheStore := method(data,
		cacheFile setContents(data)
	)
	
	cacheLoad := method(
		cf := cacheFile
		if(cf exists and(cf lastDataChangeDate secondsSinceNow < cacheTimeout), 
			//writeln("using cache"); 
			setStatusCode(200); cf contents, nil)
	)
	
//metadoc URL category Networking
//metadoc URL copyright Steve Dekorte, 2004
//metadoc URL license BSD revised
/*metadoc URL description The URL object is usefull for fetching web pages and parsing URLs. Example;
<pre>
page := URL clone setURL(\"http://www.google.com/\") fetch
</pre>
*/

	//doc URL url Returns url string.
	url := ""

	/*doc URL setURL(urlString)
	Sets the url string and parses into the protocol, host, port path, and query slots. Returns self.
	*/
	
	socketProto ::= Socket clone
	readHeader ::= nil
	statusCode ::= nil
	socket ::= nil
	username ::= nil
	password ::= nil
	usesBasicAuthentication ::= false
	redirectUrl ::= nil
	
	isSynchronous := false
	
	init := method(
		self socket := socketProto clone
		setIsSynchronous(isSynchronous)
		self requestHeaders := requestHeaders clone
	)

	setIsSynchronous := method(aBool,
		isSynchronous = aBool
		if(socket, socket setIsSynchronous(aBool))
		self
	)
	
	setTimeout := method(timeout,
		s := if(getSlot("socket"), socket, socketProto)
		s setReadTimeout(timeout)
		s setWriteTimeout(timeout)
		s setConnectTimeout(timeout)
		self
	)

	setURL := method(s,
		//if(s lastPathComponent pathExtension == "" and s containsSeq("?") == false and s endsWithSeq("/") == false, s = s .. "/")
		url = s asString
		parse
		self
	)

	host ::= nil
	protocol ::= nil

	unparse := method(
		if(host) then(
			d := if(protocol, protocol .. "://", "") .. host .. if(port and port != 80, ":" .. port, "")
		) else(
			d := ""
		)
		self url := d .. path .. if(query, "?" .. query, "")
		url
	)

	/*doc URL escapeString(aString)
	Returns a new String that is aString with the appropriate characters replaced by their URL escape codes.
	*/

	escapeString := method(u,
		u percentEncoded
	)

	/*doc URL unescapeString(aString)
	Returns a new String that is aString with the URL escape codes replaced by the appropriate characters.
	*/

	unescapeString := method(u,
		EvOutRequest decodeUri(u)
	)

	//doc URL referer Returns the referer String or nil if not set.
	//doc URL setReferer(aString) Sets the referer. Returns self.
	referer ::= nil

	//doc URL clear Private method to clear the URL's parsed attributes.
	clear := method(
		self do(
			protocol := nil
			host  := nil
			port  := 80
			path  := "/"
			query := nil
			error := nil
			request := nil
		)
	)

	//doc URL parse Private method to parse the url.
	parse := method(
		clear

		if(url containsSeq("://")) then(
			names := list("protocol", "host", "path", "query")
			parts := url orderedSplit("://", "/", "?")
			parts foreach(i, v, self setSlot(names at(i), v))
			if(host containsSeq(":"),
				port = host afterSeq(":") asNumber
				host = host beforeSeq(":")
			)
		) else(
			path = url
		)

		if(port == nil, port = 80)
		if(path == nil, path = "/")
		if(protocol and path and path beginsWithSeq("/") not, path = "/" .. path)
		if(protocol == nil, protocol = "http")
		request = if(query, path .. "?" .. query, path)
	)

	//doc URL setRequest(requestString) Private method to set the url request.
	setRequest := method(r,
		request = r
		path = request
		query = if(request, request afterSeq("?"), nil)
	)

	//doc URL with(urlString) Returns a new URL instance for the url in the urlString.
	with := method(s, fromURL,
		u := self clone setURL(s)
		if(fromURL,
			if(u host == nil, u setHost(fromURL host))

			if(u request beginsWithSeq("/") not,
				u setRequest(Path with(fromURL path pathComponent, u path))
			)
		)
		u
	)
	
	fetchWithDelegate := method(delegate,
		result := self fetch
		delegate urlFetched(self, result)
		result
	)

	fetchAndFollowRedirect := method(
		v := self fetch
		if(statusCode == 302 or statusCode == 301,
		 	v = URL with(self responseHeaders at("Location")) setCacheOn(false) fetch
		)
		v
	)
	
	evFetchHttp := method(
		//writeln("evFetchHttp")
		con := EvConnection clone setAddress(host) setPort(port) connect
		//writeln("con = ", con)
		r := con newRequest setUri(url) 
		//writeln("request = ", r)
		r requestHeaders = self requestHeaders
		//writeln("request send")
		r send
		self statusCode := r responseCode
		self responseHeaders := r responseHeaders
		//writeln("responseHeaders keys = ", responseHeaders keys)
		//writeln("evFetch  got ", r data size, " bytes")
		r data
	)

	lastRedirectUrl := method(
		//writeln("lastRedirectUrl ", url)
		if(redirectUrl, redirectUrl lastRedirectUrl, self)
	)
	
	//doc URL fetch Fetches the url and returns the result as a Sequence. Returns an Error, if one occurs.
	fetch := method(url, redirectCount,
		if(redirectCount not, redirectCount = 0)
		//if(isSynchronous not, Exception raise("URL not synchronous"))
		if(url, setURL(url))
		//writeln("URL fetch: ", self url type, ": '", self url, "'")
		if(protocol == "http", 
			v := fetchHttp
			//writeln("URL fetch, statusCode: ", statusCode)
			if(followRedirects and(statusCode == 302 or statusCode == 301),
			 	if(redirectCount >= maxRedirects, 
					//writeln("MAX REDIRECTS(" .. redirectCount .. ") on " .. url)
			 		return Error with("max redirects")
				)
				newUrl := self responseHeaders at("Location")
				if(redirectUrlNormalizer,
					newUrl = redirectUrlNormalizer normalizeRedirectUrl(newUrl)
				)
				//writeln("REDIRECT TO ", newUrl)
				self setRedirectUrl(childUrl(newUrl))
				
				if(self cookie, redirectUrl setCookie(self cookie))
				if(self responseCookie, redirectUrl setCookie(self responseCookie))
				
		 		v := redirectUrl fetch(nil, redirectCount + 1)
			)
			return v
		)
		Error with("Protocol '" .. protocol .. "' unsupported")
	)
	
	/*doc URL fetchWithProgress(progressBlock)
	Same as fetch, but with each read, progressBlock is called with the readBuffer 
	and the content size as parameters.
	*/
	fetchWithProgress := method(progressBlock,
		if(protocol == "http", return(fetch(getSlot("progressBlock"))))
		Error with("Protocol '" .. protocol .. "' unsupported")
	)

	/*doc URL stopFetch
	Stops the fetch, if there is one. Returns self.
	*/
	stopFetch := method(socket close; self)

	/*doc URL requestHeader 
	Returns a Sequence containing the request header that will be sent.
	*/
	
	setCookie := method(cookie, requestHeaders atPut("Cookie", cookie); self)
	cookie := method(requestHeaders at("Cookie"))
	responseCookie := method(responseHeaders at("Set-Cookie"))
	
	setUserAgent := method(v, requestHeaders atPut("User-Agent", v); self)
	
	requestHeaders := Map clone
	requestHeaders atPut("User-Agent", "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6)")
	requestHeaders atPut("Connection", "close")
	//requestHeaders atPut("Referer", "")
	requestHeaders atPut("Accept", "*/*")
			
	requestHeader := method(
		header := Sequence clone
		//write("request = [", request, "]\n")
		header appendSeq("GET ", request," HTTP/1.1\r\n")
		header appendSeq("Host: ", host, if(port != 80, ":" .. port, ""), "\r\n")
		if(referer, header appendSeq("Referer: ", referer, "\r\n"))
		if(usesBasicAuthentication,
			header appendSeq("Authorization: Basic ", list(username, password) join(":") asBase64 exSlice(0, -1), "\r\n")
		)

		requestHeaders foreach(k, v,
			header appendSeq(k, ":", v, "\r\n")
		)
		
		//header appendSeq("User-Agent: Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6\r\n")
		//header appendSeq("User-Agent: curl/7.18.0 (i386-apple-darwin9.2.0) libcurl/7.18.0 zlib/1.2.3\r\n")
		//header appendSeq("Host: ", host, ":", port, "\r\n")
		//header appendSeq("Connection: close\r\n")
		
		//header appendSeq("Accept: */*\r\n")
		//header appendSeq("Accept: text/html\r\n")
		//header appendSeq("Accept: text/html; q=1.0, text/*; q=0.8, image/gif; q=0.6, image/jpeg; q=0.6, image/*; q=0.5, */*; q=0.1\r\n")
		//header appendSeq("Accept: text/*; image/*;\r\n")
		//header appendSeq("Accept-Encoding: gzip, deflate\r\n")
		//header appendSeq("Accept-Language: en\r\n\r\n")
		header appendSeq("\r\n")
		header
	)

	//doc URL headerBreaks Private list of valid header break character sequences.
	headerBreaks := list("\r\r", "\n\n", "\r\n\r\n", "\n\r\n\r")
	
	//doc URL headerBreaks Private list of 2 character valid header break character sequences.
	twoCharheaderBreaks := list("\r\r", "\n\n")
	
	//doc URL headerBreaks Private list of 4 character valid header break character sequences.
	fourCharheaderBreaks := list("\r\n\r\n", "\n\r\n\r")

	//doc URL headerBreaks Private method to connect to the host and write the header.
	connectAndWriteHeader := method(header,
		//writeln("--- connectAndWriteHeader --- ")
		if(host == nil, return(Error with("No host set")))
		socket returnIfError setHost(host) returnIfError setPort(port) connect returnIfError
		socket appendToWriteBuffer(if(header, header, requestHeader)) write returnIfError
		//writeln("write [", requestHeader, "]")
	)

	//doc URL fetchRaw Fetch and return the entire response. Note: This may have problems for some request times.
	fetchRaw := method(
		connectAndWriteHeader returnIfError
		socket streamReadWhileOpen returnIfError
		socket readBuffer
	)

	//doc URL setResponseHeaderString(headerString) Private method that parses the responseHeaders.
	setResponseHeaderString := method(header,
		readHeader = header
		lines := header split("\r\n")
		self responseHeaders := Map clone
		//lines println
		statusCode = lines first split at(1) asNumber
		lines removeAt(0)
		lines foreach(line,
			responseHeaders atPut(line beforeSeq(":"), line afterSeq(":") strip)
		)
		self
	)

	//doc URL fetchHttp(optionalProgressBlock) Private method that fetches an http url.
	fetchHttp := method(progressBlock,
		if(cacheOn, r := cacheLoad; if(r, write("+"); return r))
		connectAndWriteHeader returnIfError
		r := processHttpResponse(progressBlock) returnIfError
		if(r isError not, if(cacheOn, cacheStore(r)))
		if(cacheOn and r isError not, cacheStore(r))

		contentType := responseHeaders at("Content-Type")
		if(contentType,
			//if(r containsSeq("""<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>""") or 
			if(contentType == "text/html; charset=utf-8",
				r = r asMutable setEncoding("utf8") convertToFixedSizeType
			)
			if(contentType == "text/html; charset=utf-16",
				r = r asMutable setEncoding("ucs2") convertToFixedSizeType
			)
			if(contentType == "text/html; charset=utf-32",
				r = r asMutable setEncoding("ucs4") convertToFixedSizeType
			)
		,	
			r = r asMutable setEncoding("utf8") convertToFixedSizeType
		)

		return r
	)

	hasResponseHeaders := method(
		?respondHeaders != nil
	)
	
	//doc URL processHttpResponse(optionalProgressBlock) Private method that processes http response.
	processHttpResponse := method(progressBlock,
		b := socket readBuffer
		b empty

		// read and separate the header

		//	socket write("\n\n")

		while(socket isOpen,
			socket streamReadNextChunk returnIfError
			//socket readBuffer println
			match := b findSeqs(headerBreaks)
			if(match,
				setResponseHeaderString(b exclusiveSlice(0, match index))
				b removeSlice(0, match index + match match size - 1)
				break
			)
		)

		//writeln("readHeader[", readHeader, "]")
        	//if(responseHeaders , writeln("responseHeaders [",responseHeaders keys join(","),"]"))

		if(readHeader == nil or self getSlot("responseHeaders") == nil,
			self readHeader := ""
			self responseHeaders := Map clone
		)

		contentLength := responseHeaders at("Content-Length")
		if(contentLength, 
			//writeln("contentLength: ", contentLength)
			contentLength = contentLength asNumber
		)

		while(socket isOpen,
			socket streamReadNextChunk returnIfError
			if(contentLength and b size >= contentLength, break)
			if(getSlot("progressBlock"), progressBlock(b size, contentLength) call)
		)

		if(responseHeaders at("Transfer-Encoding") == "chunked",
			//writeln("chunked encoding")
			newB := Sequence clone
			while(index := b findSeq("\r\n"),
				n := b exclusiveSlice(0, index)
				b removeSlice(0, index + 1)
				length := ("0x" .. n) asNumber
				//writeln("length = ", n, " ", length)
				newB appendSeq(b exclusiveSlice(0, length))
				b removeSlice(0, length + 1)
				//writeln("after = [", b exclusiveSlice(0, 10), "]")
			)
			b copy(newB)
		)

		//writeln("b size: ", b size)

		socket close
		if(responseHeaders at("Content-Encoding") == "gzip", Zlib; b unzip)
		b
	)

	/*doc URL fetchToFile(aFile)
	Fetch the url and save the result to the specified File object. 
	Saving is done as the data is read, which helps minimize memory usage. 
	Returns self on success or nil on error.
	*/
	fetchToFile := method(file,
		fetchHttp(block(file write(socket readBuffer); socket readBuffer empty)) returnIfError
		self
	)
	
	/*
	fetchOrFailToFilePath := method(path,
		tmpPath := "/tmp/" .. Date clone now asNumber asString md5String
		tmpFile := File with(tmpPath) open
		writeln("fetchOrFailToFilePath(", path, ")")
		e := fetchToFile(tmpFile)
		tmpFile close
		if(e isError or self statusCode != 200,
			writeln("error loading ", self url)
			tmpFile remove
		, 
			tmpFile copyToPath(path)
			tmpFile remove
		)
		e
	)
	*/
	
	fetchOrFailToFilePath := method(path,
		data := self fetch
		if(data isError or self statusCode != 200,
			writeln("error loading ", self url)
			return data
		, 
			File with(path) setContents(data)
		)
		self
	)

	childUrl := method(u,
		if(u beginsWithSeq("http") not,
			if(u beginsWithSeq("/"),
				u = Path with("http://" .. host, u)
			,
				u = Path with(url pathComponent, u)
			)
		)
		self clone setURL(u) setReferer(url) setRedirectUrl(nil)
	)

	evPost := method(parameters, headers,
		headers ifNil(headers := Map clone)
		headers atIfAbsentPut("User-Agent", "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6")
		hostHeader := if(port != 80, list(host, port) join(":"), host)
		headers atIfAbsentPut("Host", hostHeader)
		headers atIfAbsentPut("Accept", "text/html; q=1.0, text/*; q=0.8, image/gif; q=0.6, image/jpeg; q=0.6, image/*; q=0.5, */*; q=0.1")
		headers atIfAbsentPut("Content-Type", "application/x-www-form-urlencoded")
		
		if(parameters type == "Map") then(
			content := parameters keys map(k,
				Sequence with(k urlEncoded, "=", parameters at(k) urlEncoded)
			) join("&")
		) else(
			content := parameters
		)

		headers atPut("Content-Length: ", content size asString)

		//writeln("evPost ", url)
		//writeln("POST: [", content, "]")
		c := EvConnection clone setAddress(host) setPort(port) connect
		r := c newRequest setUri(request) 
		r requestHeaders = headers
		//r postData := content
		r send
		self statusCode := r responseCode
		//writeln("evFetch  got ", r data size, " bytes")
		r data
		
	)
	/*doc URL post(parameters, headers)
	Sends an HTTP post message. If parameters is a Map, its key/value pairs are 
	send as the post parameters. If parameters is a Sequence or String, it is sent directly.
	Any headers in the headers map are sent with the request.
	Returns a sequence containing the response on success or an Error, if one occurs.
	*/
	
	post := method(parameters, headers,
		connectAndWriteHeader(constructHttpHeader("POST", parameters, headers)) 
		processHttpResponse
	)
	
	put := method(parameters, headers,
	  connectAndWriteHeader(constructHttpHeader("PUT", parameters, headers))
	  processHttpResponse
	)
	
	delete := method(parameters, headers,
	  connectAndWriteHeader(constructHttpHeader("DELETE", parameters, headers))
	  processHttpResponse
	)
	
	constructHttpHeader := method(httpVerb, parameters, headers,
	  parameters ifNil(parameters = "")

		headers ifNil(headers := Map clone)
		if(usesBasicAuthentication,
			headers atPut("Authorization", "Basic " .. list(username, password) join(":") asBase64 exSlice(0, -1))
		)
		headers atIfAbsentPut("User-Agent", "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6")
		hostHeader := if(port != 80, Sequence with(host, ":", port asString), host)
		headers atIfAbsentPut("Host", hostHeader)
		headers atIfAbsentPut("Accept", "text/html; q=1.0, text/*; q=0.8, image/gif; q=0.6, image/jpeg; q=0.6, image/*; q=0.5, */*; q=0.1")
		headers atIfAbsentPut("Content-Type", "application/x-www-form-urlencoded")

		header := Sequence clone appendSeq(httpVerb .. " ", request, " HTTP/1.0\r\n")
		headers foreach(name, value,
			header appendSeq(name, ": ", value, "\r\n")
		)

		if(parameters type == "Map") then(
			content := parameters keys map(k,
				Sequence with(k urlEncoded, "=", parameters at(k) urlEncoded)
			) join("&")
		) else(
			content := parameters
		)

		header appendSeq("Content-Length: ", content size, "\r\n\r\n", content)
	)
	
	useBasicAuthentication := method(username, password,
		setUsername(username)
		setPassword(password)
		setUsesBasicAuthentication(true)
	)

	//doc URL openOnDesktop Opens the URL in the local default browser. Supports OSX, Windows and (perhaps) other Unixes.
	openOnDesktop := method(
		platform := System platform
		quotedUrl := "\"" .. url .. "\""
		if(platform == "Mac OS/X") then(
			System system("open " .. quotedUrl)
		) elseif(platform containsSeq("Windows")) then(
			result := System shellExecute("open", url)
			result ifError(result := System shellExecute("open", System getEnvironmentVariable("programfiles") .. "\\Internet Explorer\\iexplore.exe", url))
			return(result)
		) else(
			// assume generic Unix?
			System system("open " .. quotedUrl)
		)
	)

	streamDestination ::= nil
	startStreaming := method(
		fetchHttp(block(
			streamDestination write(socket readBuffer) returnIfError
			socket readBuffer empty
		))
	)

	//doc URL test Private test method.
	test := method(
		data := URL with("http://www.yahoo.com/") fetch
	)
	
	domain := method(
		parts := self host split(".") 
		parts removeLast 
		parts last	
	)

	useEv := method(
	 	self fetchHttp := self getSlot("evFetchHttp")
	 	self post := self getSlot("evPost")
	)
	
	//useEv
)

//doc Object doURL(urlString) Fetches the URL and evals it in the context of the receiver.
Object doURL := method(url, self doString(URL clone setURL(url) fetch))

//doc Sequence asURL Returns a new URL object instance with the receiver as its url string.
Sequence asURL := method(URL with(self))

//doc Map asQueryString Returns a urlencoded query string representation of this map
Map asQueryString := method(
	if(isEmpty, "", "?" .. keys sort map(k,
        k percentEncoded .. "=" .. at(k) percentEncoded
    ) join("&"))
)

//doc Map asFormEncodedBody Returns a urlencoded query string representation of this map
Map asFormEncodedBody := method(
	keys sort map(k,
        k urlEncoded .. "=" .. at(k) urlEncoded
    ) join("&")
)
