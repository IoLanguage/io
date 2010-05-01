
CFFI do(
	Structure do(
		setTypeString("{")

		init := method(
			self ptr := method(
				Pointer toType(self)
			)

			self init := method(
				self ptr := method(
					self proto ptr clone setValue(self)
				)

				self dump := method(
					(self address .. " : ") print
					self asBuffer foreach(i, (i asHex .. " ") print)
					"" println
					self
				)
			)

			self with := method(
				this := self clone
				call message clone setName("setValues") doInContext(this, call sender)
				this
			)

			self forward := method(
				self _members at(call message name)
			)
		)
	)

	Union := Structure cloneWithoutInit setTypeString("(")
)
