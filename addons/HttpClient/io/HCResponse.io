//metadoc HCResponse category Networking
//metadoc HCResponse copyright Rich Collins, 2010
//metadoc HCResponse license BSD revised
//metadoc HCResponse description Stores the result of sending an HCRequest using an HCConnection

//doc HCResponse statusCode Numeric status code.  ex: 200
//doc HCResponse statusDescription Descriptive status code ex: OK
//doc HCResponse headersAt(name) Header values associated with name
//doc HCResponse headerAt(name) First header value associated with name

HCResponse := Object clone do(
	statusCode ::= nil
	statusDescription ::= nil
	content ::= nil
	
	headers ::= method(
		headers = Map clone
	)
	
	addHeader := method(name, value,
		name = name asLowercase
		headers atIfAbsentPut(name, List clone)
		headers at(name) append(value)
		self
	)
	
	headerAt := method(name,
		headers at(name asLowercase) ?first
	)
	
	headersAt := method(name,
		headers at(name asLowercase)
	)
)