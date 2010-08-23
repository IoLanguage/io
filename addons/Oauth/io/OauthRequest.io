OauthRequest := Object clone do(
    urlPath ::= nil
    headers ::= nil
	oauthParams ::= nil
    postParams ::= nil
	queryParams ::= nil
    body ::= nil
	httpMethod ::= nil
    
	callback ::= "oob"
	consumerKey ::= nil
	nonce ::= method((Date clone now asNumber asString .. "stylous") md5String)
	signatureMethod ::= "HMAC-SHA1"
	timestamp ::= method(Date clone now asNumber asString beforeSeq("."))
	version ::= "1.0"
	token ::= nil
	verifier ::= nil

	consumerSecret ::= nil
	tokenSecret ::= nil
    
    init := method(
		setHeaders(Map clone)
		setOauthParams(Map clone)
		setPostParams(Map clone)
		setQueryParams(Map clone)
    )
    
	get := method(
		setHttpMethod("GET")
		send
	)
	
	post := method(
		setHttpMethod("POST")
		send
	)
	
	url := method(
		urlPath .. queryParams asQueryString
	)
	
	send := method(
		setupOauthParams
		calcAuthorizationHeader
		
		sc := SystemCall clone 
        sc setCommand("curl") 
        args := list("-i")
        headers foreach(k, v, args append("--header", "" .. k .. ": " .. v .. ""))
		if(httpMethod == "POST",
			args append("-d", postParams asFormEncodedBody)
		)
		args append(url)
		
		//writeln("[", args join(" "), "]")
        sc setArguments(args)
        sc run

		OauthResponse clone setData(sc stdout readLines join("\n")) parseData
	)
	
	setupOauthParams := method(
		if(callback, oauthParams atIfAbsentPut("oauth_callback", callback))
		if(consumerKey, oauthParams atIfAbsentPut("oauth_consumer_key", consumerKey))
		if(nonce, oauthParams atIfAbsentPut("oauth_nonce", nonce))
		if(signatureMethod, oauthParams atIfAbsentPut("oauth_signature_method", signatureMethod))
		if(timestamp, oauthParams atIfAbsentPut("oauth_timestamp", timestamp))
		if(version, oauthParams atIfAbsentPut("oauth_version", version))
		if(token, oauthParams atIfAbsentPut("oauth_token", token))
		if(verifier, oauthParams atIfAbsentPut("oauth_verifier", verifier))
		self
	)

	Sequence _urlEncoded := Sequence getSlot("urlEncoded")
	Sequence urlEncoded := method(asUTF8 _urlEncoded)

	Map asQueryString := method(
		if(isEmpty, "", "?" .. asFormEncodedBody)
	)

	Map asFormEncodedBody := method(
		keys sort map(k,
            k urlEncoded .. "=" .. at(k) urlEncoded
        ) join("&")
	)

    Map asOAuthHeader := method(
        "OAuth " .. keys sort map(k,
            k urlEncoded .. "=\"" .. at(k) urlEncoded .. "\""
        ) join(", ")
    )
    
    calcAuthorizationHeader := method(
        baseSeq := list(httpMethod, url urlEncoded, oauthParams merge(postParams) merge(queryParams) asFormEncodedBody urlEncoded) join("&")
		writeln(baseSeq)
        authHeader := oauthParams clone atPut("oauth_signature", SHA1 hmac(signingKey, baseSeq) asBase64 removeLast) asOAuthHeader
        self headers atPut("Authorization", authHeader)            
    )
    
    signingKey := method(consumerSecret .. "&" .. if(tokenSecret, tokenSecret, ""))	

)