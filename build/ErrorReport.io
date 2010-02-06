ErrorReport := Object clone do(
	removeFile := method(
		File clone with("errors") remove close
	)

	addError := method(error,
		error print
		Directory currentWorkingDirectory println
		File clone openForAppending("errors") write(error) close
	)
)
