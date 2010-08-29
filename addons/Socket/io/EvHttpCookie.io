EvHttpCookie := Object clone do(
	name ::= nil
	value ::= nil
	expiresDate ::= nil
	domain ::= nil
	path ::= nil
	secure ::= false
	
	response ::= nil
	
	asHeaderVal := method(
		name := name urlEncoded
		value := if(value, value urlEncoded, "")
		expires := if(expiresDate isKindOf(Date),
			expiresDate -= Duration fromNumber(expiresDate gmtOffsetSeconds)
			"expires=" .. expiresDate asString("%a, %d-%b-%Y %H:%M:%S GMT") .. "; "
		,
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

		secure := if(secure, "secure" , "")

		name .. "=" .. value .. "; " .. expires .. domain .. path .. secure
	)
	
	addToResponse := method(
		response addHeader("Set-Cookie", asHeaderVal)
		self
	)
)