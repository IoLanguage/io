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
		if(r == nil or r == "", r = "/")
		r beforeSeq("#")
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