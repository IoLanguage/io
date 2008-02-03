URL := Notifier clone do(
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

	init := method(
		self socket := socketProto clone
	)

	setTimeout := method(timeout,
		s := if(getSlot("socket"), socket, socketProto)
		s setReadTimeout(timeout)
		s setWriteTimeout(timeout)
		s setConnectTimeout(timeout)
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
		u := u clone asMutable
		u replaceSeq("%","%25")
		escapeCodes foreach(k, v, u replaceSeq(k, v))
		u asString
	)

	/*doc URL unescapeString(aString)
	Returns a new String that is aString with the URL escape codes replaced by the appropriate characters.
	*/

	unescapeString := method(u,
		u := u clone asMutable
		escapeCodes foreach(k, v, u replaceSeq(v, k))
		u replaceSeq("%25", "%")
		u asString
	)

	/*doc URL escapeCodes
	Returns a Map whose key/value pairs are special characters and their URL escape codes.
	*/

	escapeCodes := Map clone do(
		atPut(" ","%20")
		atPut("<","%3C")
		atPut(">","%3E")
		atPut("#","%23")
		//atPut("%","%25")
		atPut("{","%7B")
		atPut("}","%7D")
		atPut("|","%7C")
		//atPut("backslash","%5C")
		atPut("^","%5E")
		atPut("~","%7E")
		atPut("[","%5B")
		atPut("]","%5D")
		atPut("\`","%60")
		atPut(";","%3B")
		atPut("/","%2F")
		atPut("?","%3F")
		atPut(":","%3A")
		atPut("@","%40")
		atPut("=","%3D")
		atPut("&","%26")
		atPut("$","%24")
	)

	//doc URL referer Returns the referer String or nil if not set.
	//doc URL setReferer(aString) Sets the referer. Returns self.
	referer ::= nil

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

	setRequest := method(r,
		request = r
		path = request
		query = if(request, request afterSeq("?"), nil)
	)

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

	//doc URL fetch Fetches the url and returns the result as a Sequence. Returns an Error, if one occurs.
	fetch := method(url,
		if(url, setURL(url))
		if(protocol == "http", return(fetchHttp))
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

	/*doc URL requestHeader Returns a Sequence containing the request header that will be sent.*/

	requestHeader := method(
		header := Sequence clone
		//write("request = [", request, "]\n")
		header appendSeq("GET ", request," HTTP/1.1\r\n")
		header appendSeq("Host: ", host, ":", port, "\r\n")
		header appendSeq("Connection: close\r\n")
		header appendSeq("User-Agent: Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6\r\n")
		if(referer, header appendSeq("Referer: ", referer, "\r\n"))
		//header appendSeq("Accept: */*\r\n")
		header appendSeq("Accept: text/html; q=1.0, text/*; q=0.8, image/gif; q=0.6, image/jpeg; q=0.6, image/*; q=0.5, */*; q=0.1\r\n")
		//header appendSeq("Accept-Encoding: gzip, deflate\r\n")
		header appendSeq("Accept-Language: en\r\n")
		header appendSeq("\r\n")
		header
	)

	headerBreaks := list("\r\r", "\n\n", "\r\n\r\n", "\n\r\n\r")
	twoCharheaderBreaks := list("\r\r", "\n\n")
	fourCharheaderBreaks := list("\r\n\r\n", "\n\r\n\r")

	connectAndWriteHeader := method(
		if(host == nil, return(Error with("No host set")))
		socket setHost(host) returnIfError setPort(port) connect returnIfError
		socket appendToWriteBuffer(requestHeader) write returnIfError
	)

	fetchRaw := method(
		connectAndWriteHeader returnIfError
		socket streamReadWhileOpen returnIfError
		socket readBuffer
	)

	setReadHeader := method(header,
		readHeader = header
		lines := header split("\r\n")
		self headerFields := Map clone
		//lines println
		lines removeAt(0)
		lines foreach(line,
			headerFields atPut(line beforeSeq(":"), line afterSeq(":") strip)
		)
		self
	)

	fetchHttp := method(progressBlock,
		connectAndWriteHeader returnIfError
		processHttpResponse(progressBlock)
	)
	
	processHttpResponse := method(progressBlock,
		b := socket readBuffer
		b empty

		// read and separate the header

		while(socket isOpen,
			socket streamReadNextChunk returnIfError
			match := b findSeqs(headerBreaks)
			if(match,
				setReadHeader(b slice(0, match index))
				b removeSlice(0, match index + match match size - 1)
				break
			)
		)

		if(readHeader == nil, return(Error with("didn't find read header in [" .. b .. "]")))

		contentLength := headerFields at("Content-Length")
		if(contentLength, contentLength = contentLength asNumber)

		while(socket isOpen,
			socket streamReadNextChunk returnIfError
			if(contentLength and b size >= contentLength, break)
			if(getSlot("progressBlock"), progressBlock(b size, contentLength))
		)
		socket close

		if(headerFields at("Transfer-Encoding") == "chunked",
			newB := Sequence clone
			while(index := b findSeq("\r\n"),
				n := b slice(0, index)
				b removeSlice(0, index + 1)
				length := ("0x" .. n) asNumber
				newB appendSeq(b slice(0, length))
				b removeSlice(0, length)
			)
			b copy(newB)
		)

		if(headerFields at("Content-Encoding") == "gzip", Zlib; b unzip)
		b
	)

	/*doc URL fetchToFile(aFile)
	Fetch the url and save the result to the specified File object. 
	Saving is done as the data is read, which help minimize memory usage. 
	Returns self on success or nil on error.
	*/
	fetchToFile := method(file,
		fetchHttp(block(file write(socket readBuffer); socket readBuffer empty)) returnIfError
		self
	)

	childUrl := method(u,
		if(u beginsWithSeq("http") not,
			u = Path with(url pathComponent, u)
		)
		URL clone setURL(u) setReferer(url)
	)

	/*doc URL post(data)
	Sends an http post message. If data is a Map, it's key/value pairs are 
	send as the post parameters. If data is a Sequence or String, it is sent directly. 
	Returns a sequence containing the response on success or an Error, if one occurs.
	*/

	post := method(postdata,
		postdata ifNil(postdata = "")
		ip := Host clone setName(host) address returnIfError

		header := Sequence clone
		header appendSeq(
			"POST ", request, " HTTP/1.0\r\n",
			"User-Agent: Mozilla/4.0\r\n",
			"Host: ", host, "\r\n",
			//"Accept: */*\r\n",
			"Accept: text/html; q=1.0, text/*; q=0.8, image/gif; q=0.6, image/jpeg; q=0.6, image/*; q=0.5, */*; q=0.1",
			"Content-Type: application/x-www-form-urlencoded\r\n"
		)

		if(postdata type == "Map") then(
			buffer := Sequence clone
			postdata keys foreach(i, j,
			buffer appendSeq(escapeString(j), "=", escapeString(postdata at(j)))
			if(i < postdata size - 1, buffer appendSeq("&"))
			)
			content := buffer asString
		) else(
			content := postdata
		)

		header appendSeq("Content-Length: ", content size, "\r\n\r\n", content)

		connectAndWriteHeader returnIfError
		processHttpResponse
	)

	openOnDesktop := method(
		platform := System platform
		quotedUrl := "\"" .. url .. "\""
		if(platform == "Mac OS/X") then(
			System system("open " .. quotedUrl)
		) elseif(platform containsSeq("Windows")) then(
			System shellExecute(url)
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

	test := method(
		data := URL with("http://www.yahoo.com/") fetch
	)
)

Object doURL := method(url, self doString(URL clone setURL(url) fetch))
Sequence asURL := method(URL with(self))

