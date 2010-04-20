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