EditLine do(

	# Io wasn't complied with libedit. Fake libedit as best we can so the CLI will work
	if(EditLine hasEditLib not,
		readLine := method(prompt,
			write(prompt)
			line := File standardInput readLine
			if(line,
				line .. "\n"
			,
				nil
			)
		)

		addHistory := method(line,
			nil
		)
	)
)
