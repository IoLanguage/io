URL //load Map asQueryString

Sequence asCamelized := method(
	split("_") map(i, v, if(i > 0, v asCapitalized, v)) join
)

BitlyRequest := Object clone do(
	host ::= "api.bit.ly"
	login ::= nil
	apiKey ::= nil
	version ::= "2.0.1"
	
	headers ::= nil
	path ::= nil
	
	queryParamNames ::= nil
	queryParams ::= nil
	
	init := method(
		setQueryParams(Map clone)
		setQueryParamNames(List clone)
		setHeaders(Map clone)
	)
	
	//public
	
	results := method(
		queryParams atPut("login", login) atPut("apiKey", apiKey) atPut("version", version)
		queryParamNames foreach(name,
			if(v := self perform(name asCamelized asSymbol),
				queryParams atPut(name, v asString)
			)
		)
		queryString := queryParams asQueryString
		
		url := URL with("http://" .. host .. path .. queryString) setFollowRedirects(false)
		
		url requestHeaders atPut("User-Agent", "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_5_7; en-us) AppleWebKit/530.17 (KHTML, like Gecko) Version/4.0 Safari/530.17")
		
		data := url fetch
		result := Yajl parseJson(data)
		result _url := url
		result _data := data
		debugWriteln(url url)
		debugWriteln(data)
		result
	)
	
	resultsOrError := method(
		results := self results
		if(error := if(results type == "Map", results at("error")), Error with(error), results)
	)
	
	addQuerySlots := method(querySlotNames,
		querySlotNames split foreach(name,
			self newSlot(name asCamelized)
			queryParamNames appendIfAbsent(name)
		)
		self
	)
	
	asShorten := method(
		self\
			setPath("/shorten")\
			addQuerySlots("longUrl")
	)
	
	//http://bit.ly/info/get_metrics_data?hash=3cBK5A&scope=USER_HASH&time_frame=ALL&data_type=clicks&time_series=1&t=1247787873552
	asDailyClicks := method(
		request := self\
			setHost("bit.ly")\
			setPath("/info/get_metrics_data")\
			addQuerySlots("hash")
			
		request queryParams\
			atPut("scope", "USER_HASH")\
			atPut("time_frame", "ALL")\
			atPut("data_type", "clicks")\
			atPut("time_series", Date clone now asNumber round asString .. "000")
		
		request
	)
	
	//http://bit.ly/info/get_realtime_data?data_set=pathseries&path=3U4Djp&bitly_user=twittersales&t=1257535455868
	asMinuteClicks := method(
		request := self\
			setHost("bit.ly")\
			setPath("/info/get_realtime_data")\
			addQuerySlots("path"/*hash*/)\
			addQuerySlots("bitly_user")
			
		request queryParams\
			atPut("data_set", "pathseries")\
			atPut("t", Date clone now asNumber round asString .. "000")
		
		request
	)
)