Object do(
	tryTwitter := method(
		attempts := 0
		
		while(attempts < 3,
			e := try(
				result := call evalArgs first
			)

			if(e,
				if(e hasProto(TwitterException),
					if(e isOverloaded or e isDown or e isInternalError,
						attempts = attempts + 1
					,
						return(TwitterAccount ExceptionConditional clone setException(e))
					)
				,
					if(list("Connection reset by peer", "Timeout") detect(m, e error containsSeq(m)),
						attempts = attempts + 1
					,
						e pass
					)
				)
			,
				return(TwitterAccount ExceptionConditional clone setResult(result) setDone(true))
			)
		)
		
		e pass
	)
)