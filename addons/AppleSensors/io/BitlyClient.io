BitlyClickResult := Object clone do(
	clicks ::= 0
	time ::= nil
)

BitlyClient := Object clone do(
	login ::= nil
	apiKey ::= nil
	
	request := method(name,
		BitlyRequest clone setLogin(login) setApiKey(apiKey)
	)
	
	shorten := method(url,
		r := request asShorten setLongUrl(url) resultsOrError returnIfError
		debugWriteln(r)
		r at("results") at(url) at("shortUrl")
	)
	
	dailyClicks := method(hash,
		request asDailyClicks setHash(hash) resultsOrError returnIfError at("results") map(result,
			BitlyClickResult clone\
				setClicks(result at("clicks") asNumber)\
				setTime(Date fromString(result at("date"), "%Y-%m-%d") setGmtOffset(5*60) convertToLocal)
		)
	)
	
	minuteClicks := method(hash,
		u := URL with("http://bit.ly/info/get_realtime_data?data_set=pathseries&path=" .. hash .. "&bitly_user=" .. login .. "&t=" .. Date clone now convertToZone(5*60, Date clone now isDST) asNumber round asString .. "000")
		debugWriteln(u url)
		u requestHeaders atPut("User-Agent", "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_5_7; en-us) AppleWebKit/530.17 (KHTML, like Gecko) Version/4.0 Safari/530.17")
		body := u fetch raiseIfError
		debugWriteln(body)
		Yajl parseJson(body) raiseIfError map(r,
			cr := BitlyClickResult clone
			cr setClicks(r at("clicks") asNumber)
			cr setTime(Date clone fromString(r at("date"), "%Y-%m-%d %H:%M:%S") setGmtOffset(5*60) convertToLocal)
		)
	)
)