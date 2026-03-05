ResumableExceptionTest := UnitTest clone do(
	testBasicSignalAndResume := method(
		result := withHandler(Exception,
			block(e, resume, resume invoke(42)),
			Exception signal("need value") + 1
		)
		assertEquals(result, 43)
	)

	testAutoResume := method(
		// Handler returns value without explicit resume invoke
		result := withHandler(Exception,
			block(e, resume, 42),
			Exception signal("need value") + 1
		)
		assertEquals(result, 43)
	)

	testNoHandler := method(
		// signal with no handler falls back to raise
		e := try(Exception signal("unhandled"))
		assertEquals(e error, "unhandled")
	)

	testHandlerReRaises := method(
		e := try(
			withHandler(Exception,
				block(exc, resume, exc pass),
				Exception signal("will re-raise")
			)
		)
		assertEquals(e error, "will re-raise")
	)

	testCustomExceptionType := method(
		MyError := Exception clone do(type := "MyError")
		result := withHandler(MyError,
			block(exc, resume, resume invoke("handled")),
			MyError signal("custom error")
		)
		assertEquals(result, "handled")
	)

	testHandlerMatchesByProto := method(
		// Handler for OtherType doesn't match Exception signal
		OtherType := Exception clone do(type := "OtherType")
		result := withHandler(OtherType,
			block(exc, resume, resume invoke("wrong handler")),
			withHandler(Exception,
				block(exc, resume, resume invoke("right handler")),
				Exception signal("test")
			)
		)
		assertEquals(result, "right handler")
	)

	testNestedHandlers := method(
		// Inner handler takes priority
		result := withHandler(Exception,
			block(e, resume, resume invoke("outer")),
			withHandler(Exception,
				block(e, resume, resume invoke("inner")),
				Exception signal("test")
			)
		)
		assertEquals(result, "inner")
	)

	testMultipleSignals := method(
		// Handler handles multiple signals from same body
		count := 0
		result := withHandler(Exception,
			block(e, resume,
				count = count + 1
				resume invoke(count)
			),
			a := Exception signal("first")
			b := Exception signal("second")
			list(a, b)
		)
		assertEquals(result at(0), 1)
		assertEquals(result at(1), 2)
	)

	testSignalInsideTry := method(
		// Handler installed outside try is visible inside
		result := withHandler(Exception,
			block(e, resume, resume invoke(99)),
			e := try(Exception signal("inside try"))
			// Signal was handled (resumed), no exception
			if(e, "exception", "no exception")
		)
		assertEquals(result, "no exception")
	)

	testRaiseUnchanged := method(
		// raise still works as before (non-resumable)
		e := try(Exception raise("fatal"))
		assertEquals(e error, "fatal")
	)
)
