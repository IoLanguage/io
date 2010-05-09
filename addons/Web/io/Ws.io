Ws := HttpServer clone do(
    setPort(8090)
    setHost("0.0.0.0")
    renderResponse := method(request, response,
        response body = "arsddstddstdtttttt"
        self logger info("sending response")
    )
)
