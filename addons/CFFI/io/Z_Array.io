
CFFI do(
	Array do(
		setTypeString("[")

		init := method(
			self ptr := method(
				Pointer toType(self)
			)

			self init := method(
				self ptr := method(
					self proto ptr clone setValue(self)
				)

				self squareBrackets := method(
					self at(call message argsEvaluatedIn(call sender) first)
				)

			)

			self setValues := method(
				if(self address isNil not,
					args := call message argsEvaluatedIn(call sender)
					size := self size / self arrayType size
					for(i, 0, size - 1, self atPut(i, args at(i)))
					self
				,
					nil
				)
			)

			self with := method(
				this := self clone
				call message clone setName("setValues") doInContext(this, call sender)
			)
		)
	)
)
