server := HttpServer clone do(
	setPort(8090)
	renderResponse := method(request, response,
		list("path", "uri", "body") foreach(k, 
			v := request perform(k)
			response body appendSeq(k .. ": "  .. v .. "<br>")	
		)
	
		response body appendSeq("<hr>")
	
		request headers keys sort foreach(k,
			v := request headers at(k)
			response body appendSeq(k .. ": "  .. v .. "<br>")
		)
	)
)

server start
