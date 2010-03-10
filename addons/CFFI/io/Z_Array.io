
CFFI do(
    Array do(
        setTypeString("[")

        init := method(
            self init := method(
                self ptr := method(
                    self proto ptr clone setValue(self)
                )
            )

            self ptr := method(
                Pointer toType(self)
            )

            self with := method(
                this := self clone
                call message setName("setValues") doInContext(this)
            )

            self squareBrackets := method(
                self at(call message argsEvaluatedIn(call sender) first)
            )

            self setValues := method(
                args := call message argsEvaluatedIn(call sender)
                for(i, 0, size - 1, self atPut(i, args at(i)))
                self
            )
        )
    )
)
