TwitterOauthSession := Object clone do(
	MD5

	baseUrl ::= "https://api.twitter.com/oauth/"
	account ::= nil
	newRequest := method(
		rp := OauthRequest clone
		rp setConsumerKey(account consumerKey)
		rp setConsumerSecret(account consumerSecret)
		rp
	)
	
	authorizeToken ::= nil
	authorizeTokenSecret ::= nil
	verifier ::= nil
	
	requestOauthAccess := method(
		requestToken
		authorize
		accessToken
	)
	
	requestToken := method(
		response := newRequest setUrlPath(baseUrl .. "request_token") post formEncodedMap
		setAuthorizeToken(response at("oauth_token"))
		setAuthorizeTokenSecret(response at("oauth_token_secret"))
        self
	)
	
	authorize := method(
        SGML
        // load user auth page
        url := baseUrl asMutable replaceSeq("https", "http") .. "authorize?oauth_token=" .. authorizeToken
        form := URL with(url) fetch asSGML

        params := Map clone
        form elementsWithName("input") foreach(e,
            k := e attributes at("name")
            v := e attributes at("value")
            if(k and v, params atPut(k, v))
        )
        
        // input user/pass into auth page form
        params atPut("session[username_or_email]", account screenName)
        params atPut("session[password]", account password)
        params removeAt("cancel")
        
        // load pin request page
        pinPage := URL with(baseUrl .. "authorize") post(params) 
        daPin := pinPage asSGML elementsWithNameAndId("div", "oauth_pin") first allText
		setVerifier(daPin asMutable strip)
        self
    )
    
	accessToken := method(
		req := newRequest clone setUrlPath(baseUrl .. "access_token")
		req setToken(authorizeToken) setTokenSecret(authorizeTokenSecret) setVerifier(verifier)
        response := req post formEncodedMap
		account setToken(response at("oauth_token"))
		account setTokenSecret(response at("oauth_token_secret"))
        self
	)
)