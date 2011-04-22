//metadoc HCRequest category Networking
//metadoc HCRequest copyright Rich Collins, 2010
//metadoc HCRequest license BSD revised
//metadoc HCRequest description State describing an HTTP request to be sent by an HCConnection

//doc HCRequest with(aUrl) Clones HCResponse and sets its state based on aUrl (HCUrl)
//doc HCRequest httpMethod The HTTP method to be sent
//doc HCRequest host The host to send this request to
//doc HCRequest port The port connect to when sending this request
//doc HCRequest resource The resource to request
//doc HCRequest connection Clone HCConnect and set its state based on this HCRequest
//doc HCRequest hasHeader(name) Returns true if this request has a header with name
//doc HCRequest setHeader(name, value) Sets header with name to value
//doc HCRequest headerAt(name) Returns the value for header with name
// doc HCRequest setBody(string) Sets the body to string and changes the "Content-Length" header accordingly
// doc HCReqeust body The message body

HCRequest := Object clone do(
	url ::= nil
	
	httpMethod ::= "GET"
	
	headers ::= method(
		headers = Map clone
		headers atPut("User-Agent", "Io-HttpClient http://iolanguage.com/")
		headers atPut("Accept", "*/*")
		headers atPut("Connection", "close")
	)
	
        messageBody ::= nil

	with := method(url,
		self clone setUrl(url)
	)
	
	host := method(
		url host
	)
	
	port := method(
		url port
	)
	
	resource := method(
		url resource
	)
	
	connection := method(
		HCConnection clone setRequest(self)
	)
	
	hasHeader := method(name,
		headers hasKey(name)
	)
	
	setHeader := method(name, value,
		headers atPut(name, value)
	)
	
	headerAt := method(name,
		headers at(name)
	)

        setBody := method(string,
            messageBody := string
            if(messageBody isNil,
                headers removeAt("Content-Length")
                return
            )

            setHeader("Content-Length", messageBody size asString)
        )

        body := method(
            messageBody ifNilEval(return "")
            return messageBody
        )
)
