
Sequence hasSlot("splitLines") ifFalse(
    Sequence splitLines := method(
        lineStart := 0
        resultList := List clone
        
        nextCrIndex := nil
        nextLfIndex := nil

        while(lineStart < size,
            nextCrIndex = findSeq("\r", lineStart)
            nextLfIndex = findSeq("\n", lineStart)

            if(nextCrIndex and nextLfIndex,
                if(nextCrIndex < nextLfIndex) then(
                    nextLfIndex = nil
               ) else(
                    nextCrIndex = nil
               )
           )

            if(nextCrIndex) then(
                resultList append(slice(lineStart, nextCrIndex))
                if(at(nextCrIndex + 1) == 10) then(
                    lineStart = nextCrIndex + 2
               ) else(
                    lineStart = nextCrIndex + 1
               )
           ) elseif(nextLfIndex,
                resultList append(slice(lineStart, nextLfIndex))
                lineStart = nextLfIndex + 1
           ) else(
                break
           )
       )
        
        resultList append(slice(lineStart, size))

        return resultList
   )
)


HttpServer := Object clone do(
	type := "HttpServer"
	docCategory("Networking")
	docDescription("""Usage example;
	<pre>
	HttpServer serve (port, HttpRequest,
		page := Object clone
		page host := HttpRequest mySocket host
	
		// Tell the send method to send its output to this server
		request := HttpRequest
		mySend := method (request send (self))
		
		HttpRequest sendResponse (200, "OK")
		HttpRequest sendHeader ("Content-type", "text/HTML")
		HttpRequest endHeaders ()
		query := HttpRequest queryArgs
	
		action := query at ("action") or "show"
		if (action type != "Sequence", action := "show")
		page perform (action)
			// ... which calls other methods, which you have to write yourself!
			// These methods can write web pages by calling mySend (text),
			// and can get query items by calling e.g. query at ("password").
		
		HttpRequest close
	)
	</pre>
	""")
		HttpRequest := Object clone
		
		HttpRequest do(
			lineMode := 1
		
			mandatorySpacePattern := "[ \t]+"
			optionalSpacePattern := "[ \t]*"
			tokenPattern := "([^\\x00-\\x1F\\x7F()<>@.;:\\\\\"/[\\]?={} \t]+)"
			quotedPattern := "\"((?:\\\\\"|[^\"])+\)\""
			
			headerPattern := "^" .. tokenPattern .. optionalSpacePattern .. ":" .. optionalSpacePattern .. "(.*)$"
			commandPattern := "^" .. tokenPattern .. mandatorySpacePattern .. "(.*)$" 
			pairPattern := "^" .. optionalSpacePattern .. tokenPattern .. optionalSpacePattern .. "=" .. optionalSpacePattern .. "(?:" .. tokenPattern .. "|" .. quotedPattern .. ")(?:;(.*))?$"
			queryPattern := "^([^?]*).(.*)$"
		
			headerRegex := Regex clone setPattern(headerPattern)
			commandRegex := Regex clone setPattern(commandPattern)
			pairRegex := Regex clone setPattern(pairPattern)
			queryRegex := Regex clone setPattern(queryPattern)
		
		   init := method(
				self sentCookies := Map clone
		   )
		
		   lineBuffer := method(
				self lineBuffer := Sequence clone
		   )

		   rawHeaders := method(
				self rawHeaders := List clone
		   )

		   headers := method(
				self headers := Map clone
				self rawHeaders foreach(v,
					self headers atPut(v at(0) asString, v at(1) asString)
			   )
				return self headers
		   )

		   sentBuffer := method(
				self sentBuffer := Sequence clone
		   )

		   queryArgs := method(
				self parseQuery; return queryArgs
		   )

		   queryPath := method(
				self parseQuery; return queryPath
		   )

		   queryCookies := method(
				self parseQueryCookies; return queryCookies
		   )

		   handleSocket := method(aSocket,
				self mySocket := aSocket
		
				while(self mySocket isOpen,
					if(self mySocket streamReadNextChunk,
						input :=  self mySocket readBuffer 
						// writeln(input)
						self handleInput(input)
				   )
			   )
		   )

		   handleInput := method(readBuffer,
				if(lineMode,
					lineBuffer appendSeq(readBuffer)
					readBuffer empty
					
					readLines := lineBuffer asString splitLines
		
					rest := readLines pop
		
					readLines foreach(line, self currentParser(line))
		
					if(lineMode,
						self lineBuffer := rest asBuffer
				   ) else(
						self currentParser(rest asString)
				   )
			   ) else(
					self lineBuffer appendSeq(readBuffer)
					self currentParser(readBuffer asString)
			   )
		   )

		   handleRequest := method(request,
				self close
		   )
			

		   currentParser := method(line,
				self chainParser("commandParser", line)
		   )

		   chainParser := method(parserName, line,
				self currentParser := self getSlot(parserName)
				self currentParser(line)
		   )

		   commandParser := method(line,
				self commandRegex setString(line)
				if(self commandRegex hasMatch,
					command := self commandRegex firstMatch
					self command := command at(1)
					self arguments := command at(2) splitNoEmpties(" ")
			   ) else(
					self chainParser("headerParser", line)
			   )
		   )
		   
		   headerParser := method(line,
				self headerRegex setString(line)
				if(self headerRegex hasMatch,
					header := self headerRegex firstMatch
					self rawHeaders append(
						List clone append(header at(1), header at(2))
				   )
			   ) else(
					self chainParser("completeParser", line)
			   )
		   )

		   completeParser := method(line,
				if(line size == 0,
					self determineCommand
			   ) else(
					Lobby write(
						"Server does not understand http command: '", 
						line, "'\n" 
				   )
			   )
		   )
		   
		   determineCommand := method(
				if(command == "GET",
					self handleRequest(self)
			   ) else(
					if(command == "POST",
						self lineBuffer empty
						self currentParser := self getSlot("formParser")
						self contentLength := self headers at(
							"Content-Length" 
					   )
						self lineMode = nil
				   )
			   )
		   )
		   
		   formParser := method(data,
				lineBuffer appendSeq(data)
				if(lineBuffer size >= contentLength,
					self queryPath := self arguments at(0)
					self queryArgs := CGI parseString(
						lineBuffer asString
				   )
					self handleRequest(self)
					lineBuffer empty
			   )
		   )
		   
		   parseQuery := method(
				// This method is invoked lazily by queryPath and queryArgs
				query := self arguments at(0) asString
				if(query findSeq("?"),
					self queryRegex setString(query)
					self queryPath := self queryRegex firstMatch at(1)
					self queryArgs := CGI parseString(
						self queryRegex firstMatch at(2) 
				   )
			   ) else(
					self queryPath := query 
					self queryArgs := Map clone
			   )
		   )
		   
		   parseQueryCookies := method(
				cookieHeader := self headers at("Cookie")
		
				self queryCookies := Map clone
		
				loop(
					cookieHeader ifNil(return)
					results := self pairRegex setString(cookieHeader) firstMatch
					results ifNil(return)
					
					if(results at(2) == "",
						self queryCookies atPut(
							results at(1), results at(3) 
					   )
				   ) else(
						self queryCookies atPut(
							results at(1), results at(2) 
					   )
				   )
					cookieHeader := results at(4)
			   )
		   )
			sendCookie := method(key, value, 
				sentCookies atPut(key, value)
		   )
			sendList := method(data,
				data foreach(v, self sentBuffer appendSeq(v))
		   )
			send := method(           	
				sendList(call message argsEvaluatedIn(call sender))
		   )
			sendHeader := method(key, value,
				self send(key, ": ", value, "\r\n")
		   )
			endHeaders := method(
				self send("\r\n")
		   )
			sendResponse := method(code, message,
				self responseCode := code
				self responseMessage := message
		   )
			flush := method(
				self removeSlot("sentBuffer")
		   )
			done := method(
				self mySocket write(
					"HTTP/1.1 ", self responseCode asString, " ", 
					self responseMessage, "\r\n" 
			   )
				self sentCookies foreach(key, value,
					self mySocket write(
						"Set-cookie: ", key, "=", value, "\r\n" 
				   )
			   )
				self mySocket write(sentBuffer asString)
				// writeln(sentBuffer asString) // debug
				self flush
		   )
			close := method(
				self hasSlot("sentBuffer")ifTrue(self done)
				self mySocket close
		   )
		)
		
		serve := method(httpPort,
			if(call message arguments size == 2,
				// We've been given a block.
				requestHandler := call message arguments at(1)
		   ) else (
				// We assume we've been given a slot-name and a message.
				requestHandler := block(nil)
				getSlot("requestHandler")setArgumentNames(
					List clone append(call message arguments at(1) name)
			   )
				getSlot("requestHandler")setMessage(
					call message arguments at(2) 
			   )
				getSlot("requestHandler")setScope(call sender)
		   )
			
			requestProto := HttpRequest clone
			requestProto handleRequest := getSlot("requestHandler")
		
			return self serveWithProto(httpPort, requestProto)
		)
		
		serveWithProto := method(httpPort, requestProto,
			server := Server clone setPort(httpPort)
			server requestProto := requestProto
			server handleSocket := method(aSocket,
				aSocket setBytesPerRead(10000000)
				requestProto clone handleSocket(aSocket)
		   )
			server @start
			return server
		)
)
