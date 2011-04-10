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
				if(nextCrIndex < nextLfIndex,
					nextLfIndex = nil,
					nextCrIndex = nil
				)
			)

			if(nextCrIndex,
				resultList append(exSlice(lineStart, nextCrIndex))
				if(at(nextCrIndex + 1) == 10,
					lineStart = nextCrIndex + 2
				,
					lineStart = nextCrIndex + 1
				)
			,
				if(nextLfIndex,
					resultList append(exSlice(lineStart, nextLfIndex))
					lineStart = nextLfIndex + 1
				,
					break
				)
			)
		)

		resultList append(exSlice(lineStart, size))

		return resultList
	)
)

WebRequest := Object clone

WebRequest do(
	lineMode := 1

	mandatorySpacePattern := "[ \t]+"
	optionalSpacePattern := "[ \t]*"
	tokenPattern := "([^\\x00-\\x1F\\x7F()<>@.;:\\\\\"/[\\]?={} \t]+)"
	quotedPattern := "\"((?:\\\\\"|[^\"])+\)\""

	headerPattern := "^" .. tokenPattern .. optionalSpacePattern .. ":" .. optionalSpacePattern .. "(.*)$"
	commandPattern := "^" .. tokenPattern .. mandatorySpacePattern .. "(.*)$"
	pairPattern := "^" .. optionalSpacePattern .. tokenPattern .. optionalSpacePattern .. "=" .. optionalSpacePattern .. "(?:" .. tokenPattern .. "|" .. quotedPattern .. ")(?:;(.*))?$"
	queryPattern := "^([^?]*).(.*)$"

	headerRegex := Regex clone with(headerPattern)
	commandRegex := Regex clone with(commandPattern)
	pairRegex := Regex clone with(pairPattern)
	queryRegex := Regex clone with(queryPattern)

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
				self lineBuffer := rest asBuffer,
				self currentParser(rest asString)
			)
		,
			self lineBuffer appendSeq(readBuffer)
			self currentParser(readBuffer asString)
		)
	)

	handleRequest := method(request,
		self sendResponse (200, "OK")
		self sendHeader ("Content-type", "text/HTML")
		self endHeaders ()
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
		match := line matchesOfRegex(self commandRegex) all first
		if(match,
			command := match
			self command := command at(1)
			self arguments := command at(2) splitNoEmpties(" ")
		,
			self chainParser("headerParser", line)
		)
	)

	headerParser := method(line,
		match := line matchesOfRegex(self headerRegex) all first
		if(match,
			header := match
			self rawHeaders append(
				List clone append(header at(1), header at(2))
			)
		,
			self chainParser("completeParser", line)
		)
	)

	completeParser := method(line,
		if(line size == 0,
			self determineCommand,
			Lobby write(
				"Server does not understand http command: '",
				line, "'\n"
			)
		)
	)

	determineCommand := method(
		if(command == "GET",
			self handleRequest(self)
		,
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
			match := query matchesOfRegex(self queryRegex) all first
			self queryPath := match at(1)

			self queryArgs := CGI parseString(
				match at(2)
			)
		,
			self queryPath := query
			self queryArgs := Map clone
		)
	)

	parseQueryCookies := method(
		cookieHeader := self headers at("Cookie")

		self queryCookies := Map clone

		loop(
			cookieHeader ifNil(return)
			results := cookieHeader matchesOfRegex(pairRegex) all first
			results ifNil(return)

			if(results at(2) == "",
				self queryCookies atPut(
					results at(1), results at(3)
				)
			,
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
			"HTTP/1.1 " .. self responseCode asString .. " " .. self responseMessage .. "\r\n"
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
		self hasSlot("sentBuffer") ifTrue(self done)
		self mySocket close
	)
)

WebServerDefaultRequestHandler := WebRequest clone do(
	handleRequest := method(request,
		self sendResponse (200, "OK")
		self sendHeader ("Content-type", "text/HTML")
		self endHeaders ()
		self send("<html><title>Test Server</title><p>Blah blah blah</p></html>")
		self close
	)
)

WebServer := Server clone do(
	setHost("127.0.0.1")
	setPort(8000)
	handleSocket := method(aSocket,
		WebServerDefaultRequestHandler clone @handleSocket(aSocket)
	)
)

WebServer start
