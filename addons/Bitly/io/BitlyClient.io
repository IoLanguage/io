BitlyDailyClickResult := Object clone do(
	clicks ::= 0
	date ::= nil
)

BitlyClient := Object clone do(
	login ::= nil
	apiKey ::= nil
	
	request := method(name,
		BitlyRequest clone setLogin(login) setApiKey(apiKey)
	)
	
	shorten := method(url,
		request asShorten setLongUrl(url) resultsOrError\
			returnIfError\
			at("results")\
				at(url)\
					at("shortUrl")
	)
	
	dailyClicks := method(hash,
		request asDailyClicks setHash(hash) resultsOrError returnIfError at("results") map(result,
			BitlyDailyClickResult clone\
				setClicks(result at("clicks") asNumber)\
				setDate(Date fromString(result at("date"), "%Y-%m-%d"))
		)
	)
)