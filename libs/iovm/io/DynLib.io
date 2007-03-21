DynLib do(
	init := method(
		self forward := method(
			self performWithArgList("call", list(call message name, call message arguments map(x, self doMessage(x))) flatten)
		)
	)
)
