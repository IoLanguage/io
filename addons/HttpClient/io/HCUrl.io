//metadoc HCUrl category Networking
//metadoc HCUrl copyright Rich Collins, 2010
//metadoc HCUrl license BSD revised
/*metadoc HCUrl description A replacement for URL that delegates fetching requests to HCConnection
<pre>
HCUrl with("http://www.apple.com/") get println
HCUrl with("http://www.apple.com/") getResponse statusCode println
</pre>
*/

//doc HCUrl with(urlSeq) Returns a clone of HCUrl with its state set by parsing urlSeq
//doc HCUrl url Returns url's host
//doc HCUrl port Returns url's port
//doc HCUrl get Creates an HCConnection based on this url, sends a request and returns the content of the response
//doc HCUrl getResponse Creates an HCConnection based on this url, sends a request and returns the response

//doc Sequence asUrl Shortcut for HCUrl with(aUrlSeq)



HCUrl := Object clone do(
	
	
	host ::= nil
	
	urlSeq ::= nil
	
	url := method(
		URL with(urlSeq)
	)
	
	with := method(urlSeq,
		setUrlSeq(urlSeq)
	)
	
	host := method(
		url host
	)
	
	port := method(
		url port
	)
	
	resource := method(
		r := urlSeq afterSeq("//") ?afterSeq("/")
		if(r == nil, r = "")
		"/"..(r) beforeSeq("#")
	)
	
	get := method(
		getResponse content
	)
	
	getResponse := method(
		HCRequest with(self) connection sendRequest response
	)
)

Sequence do(
	asUrl := method(
		HCUrl with(self)
	)
)