ErrorReport := Object clone do(
	removeFile := method(
		File clone with("errors") remove close
	)

	addError := method(error,
		error print
		File clone openForAppending("errors") write(error) close
	)
)
