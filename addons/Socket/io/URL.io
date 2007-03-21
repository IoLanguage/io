
URL := Notifier clone do(
	docCategory("Networking")
	docCopyright("Steve Dekorte", 2004)
	docLicense("BSD revised")
	docDescription("The URL object is usefull for fetching web pages and parsing URLs. Example;
	<pre>
	page := URL clone setURL(\"http://www.google.com/\") fetch
	</pre>")

    docSlot("url", "Returns url string.")
    url := ""

    docSlot("setURL(urlString)",
    "Sets the url string and parses into the protocol, host, port path, and query slots. Returns self.")

	newSlot("socketProto", Socket clone)
	newSlot("readHeader")
	
	init := method(
		self socket := socketProto clone
	)
	
    setURL := method(s, 
    	//if(s lastPathComponent pathExtension == "" and s containsSeq("?") == false and s endsWithSeq("/") == false, s = s .. "/")
        url = s asString
        parse
        self
    )
    
    docSlot("error", "Returns error string if it has been set or nil otherwise.")
    newSlot("error")
    newSlot("host")
    newSlot("protocol")
    
    unparse := method(
		if(host) then(
    		d := if(protocol, protocol .. "://", "") .. host .. if(port and port != 80, ":" .. port, "")
    	) else(
    		d := ""
    	)
    	self url := d .. path .. if(query, "?" .. query, "")
    	url
    )
    
    docSlot("escapeString(aString)", "Returns a new String that is aString with the appropriate characters replaced by their URL escape codes.")

    escapeString := method(u,
        u := u clone asMutable 
        u replaceSeq("%","%25")
        escapeCodes foreach(k, v, u replaceSeq(k, v))
        u asString
    )

    docSlot("unescapeString(aString)", "Returns a new String that is aString with the URL escape codes replaced by the appropriate characters.")
    
    unescapeString := method(u,
        u := u clone asMutable 
        escapeCodes foreach(k, v, u replaceSeq(v, k))
        u replaceSeq("%25", "%")
        u asString
    )

    docSlot("escapeCodes", "Returns a Map whose key/value pairs are special characters and their URL escape codes.")
    
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

    docSlot("referer", "Returns the referer String or nil if not set.")
    docSlot("setReferer(aString)", "Sets the referer. Returns self.")
    newSlot("referer", nil)

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

    docSlot("fetch", "Fetches the url and returns the result as a Sequence. Raises an exception if an error occurs.")
    
    fetch := method(url,
        if(url, setURL(url))
        if(protocol == "http", return fetchHttp)
        Exception raise("protocol '" .. protocol .. "' unsupported")
    )

    docSlot("fetchWithProgress(progressBlock)", "Same as fetch, but with each read, progressBlock is called with the readBuffer and the content size as parameters.")

    fetchWithProgress := method(progressBlock,
        if(protocol == "http", return fetch(getSlot("progressBlock")))
        Exception raise("protocol '" .. protocol .. "' unsupported")
    )

    docSlot("stopFetch", "Stops the fetch, if there is one. Returns self.")

    stopFetch := method(socket close; self)

    docSlot("requestHeader", "Returns a Sequence containing the request header that will be sent.")

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
    
    fetchRaw := method(
		socket setHost(host) setPort(port) connect
        if(socket error, error := socket error; return nil)
        socket streamWrite(requestHeader)
        while(socket streamReadNextChunk, nil)
        socket readBuffer
    )
    
    setReadHeader := method(header,
    	readHeader = header
    	lines := header split("\r\n")
    	self headerFields := Map clone
    	//lines println
    	lines removeAt(0)
    	lines foreach(line, 
    		//writeln(line)
    		headerFields atPut(line beforeSeq(":"), line afterSeq(":") strip)
    	)
    	self
    )
    
    fetchHttp := method(progressBlock,
        if(host == nil, Exception raise("no host set"))
        //writeln("host = '", host, "'")
		socket setHost(host) setPort(port) connect
        if(socket error, error := socket error; return nil)
    
        //writeln("request = [", request, "]")
        socket streamWrite(requestHeader)
        
    	b := socket readBuffer
    
    	// read and separate the header

        loop(
            more := socket streamReadNextChunk 
            match := b findSeqs(headerBreaks)
						
			if(match,
                    setReadHeader(b slice(0, match index))
                    b removeSlice(0, match index + match match size - 1)
                    more := false
                    break
            )
                        
            if(more not, break)
        )

       // writeln("URL got header [", readHeader, "]")
        
        if(readHeader == nil, Exception raise("didn't find read header in [", b, "]"))

        contentLength := headerFields at("Content-Length")
        if(contentLength, contentLength = contentLength asNumber)
        //writeln("URL contentLength = ", contentLength)
        
		while(socket isOpen and socket streamReadNextChunk, 
			if(contentLength and b size >= contentLength, writeln("break"); break) 
			if(getSlot("progressBlock"), progressBlock(b size, contentLength))
		)

        if(headerFields at("Transfer-Encoding") == "chunked",
        	index := b findSeq("\r\n")
        	newB := Sequence clone
        	while(index, 
        		n := b slice(0, index)
        		b removeSlice(0, index + 2)
        		length := ("0x" .. n) asNumber
        		newB appendSeq(b slice(0, length))
        		b removeSlice(0, length)
        		index := b findSeq("\r\n")
        	)
        	b copy(newB)
        )
        
        if(socket error, writeln(socket error); return)
        socket close
        //writeln("b = ", b)
        if(headerFields at("Content-Encoding") == "gzip", Zlib; b unzip)
        b
    )

	docSlot("fetchToFile(aFile)", "fetch the url and save the result to the specified File object. Saving is done as the data is read, which help minimize memory usage. Returns self on success or nil on error.")

    fetchToFile := method(file,
        fetchHttp(block(file write(socket readBuffer); socket readBuffer empty))   
        self
    )
    
    childUrl := method(u, 
        if(u beginsWithSeq("http") not, 
            u = Path with(url pathComponent, u)
        )
        URL clone setURL(u) setReferer(url)
    )

    docSlot("post(data)", "Sends an http post message. If data is a Map, it's key/value pairs are send as the post parameters. If data is a Sequence or String, it is sent directly. Returns self on success or nil on error.")

    post := method(postdata,
        ip := Host clone setName(host) address
        
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
        
        socket setHost(ip) setPort(port) 
        socket connect 
        
        if(socket error, 
            error = socket error
            return nil
        )
         
        socket streamWrite(header)
        while (socket streamReadNextChunk, socket readBuffer empty)
        socket close
        if(socket error, self setError(socket error); return nil)     
        self
    )

    openOnDesktop := method(
        platform := System platform
        if(platform == "Mac OS/X") then(
            System system("open " .. url)
        ) elseif(platform == "Windows NT") then(
            System system("cmd /c start " .. url)
        ) elseif(platform == "Windows 9X") then(
            System system("command /c start " .. url)
        ) else(
            // assume generic Unix?
            System system("open " .. url)
        )
    )
    
    
	newSlot("streamDestination")
	startStreaming := method(
		fetchHttp(block(
			streamDestination write(socket readBuffer)
			socket readBuffer empty
		))
	)

	test := method(
		data := URL with("http://www.yahoo.com/") fetch 
		//writeln("URL test fetched ", data size, " bytes")
	)
)

Object doURL := method(url, self doString(URL clone setURL(url) fetch))
Sequence asURL := method(URL with(self))

