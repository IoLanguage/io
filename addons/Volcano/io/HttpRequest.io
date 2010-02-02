HttpRequest := Object clone do(
	uri ::= nil
	fragment ::= nil
	path ::= nil
	queryString ::= nil
	version ::= nil
	requestMethod ::= nil
	body ::= nil
	fields ::= nil
	
	headers := method(
		fields
	)
	
	cookies := method(
		raw := fields at("COOKIE")
		parsed := Map clone

		raw ?splitNoEmpties(";") foreach(cook,
			cook strip
			parsed atPut(cook beforeSeq("=") strip, decodeUrlParam(cook afterSeq("=") strip))
		)

		return parsed
	)
	
	decodeUrlParam := method(s,
		/*
		t := s asUTF8 asMutable
		t replaceSeq("+", " ") replaceMap(urlCode2Char) replaceSeq("%25", "%")
		*/
		URL unescapeString(s)
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
	
	postData := method(
		postData = if(requestMethod == "POST", body, nil)
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
	
	parseStringtoValues := method(q,
		if(q == nil, return Map clone)
		q = q asMutable replaceSeq("+", " ")

		form := Map clone
		q splitNoEmpties("&") foreach(i, v,
			key := v beforeSeq("=")
			value := v afterSeq("=")
			
			form atPut(key, URL unescapeString(value))
		)

		return form
	)
	
	parseString := getSlot("parseStringtoValues")
	
	parseStringToLists := method(q,
		if(q == nil, return Map clone)
		q = q asMutable replaceSeq("+", " ")

		form := Map clone
		q splitNoEmpties("&") foreach(i, v,
			key := v beforeSeq("=")
			value := v afterSeq("=")
			
			form atIfAbsentPut(key, List clone)
			form at(key) append(URL unescapeString(value))
		)

		return form
	)
	
	setParseAsLists := method(aBool,
		if(aBool, 
			self parseString := self getSlot("parseStringToLists")
		,
			self parseString := self getSlot("parseStringtoValues")
		)
		self
	)
	
	parameters := method(
		parameters = Map clone addKeysAndValues(postParameters keys, postParameters values) addKeysAndValues(getParameters keys, getParameters values)
	)
	
	getParameters := method(
		q := queryString
		getParameters = if(q == nil, Map clone, self parseString(q))
	)
	
	contentType := method(
		contentType = fields at("CONTENT_TYPE")
	)
	
	postParameters := method(
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
						k := kv at(0) strip asLowercase
						v := kv at(1) ?strip("\"")
						subheaders atPut(k, v)
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
		postParameters = form
	)
)