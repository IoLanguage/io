
Twitter := Object clone do(
	forward := method(
		Exception raise("There is no Twitter proto.  See TwitterAccount instead.")
	)
)
