server := HttpServer clone do(
  setPort(8080)

  renderResponse := method(request, response,
    request headers foreach(headerName, headerValue,
    	response body appendSeq(headerName .. ": "  .. headerValue .. "<br>")
    )
  )
)

server start
