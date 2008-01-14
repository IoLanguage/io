System do(
	launchScript := nil
	docSlot("launchScript", "Returns the path of the io file run on the command line. Returns nil if no file was run.")

	ioPath := installPrefix asMutable appendPathSeq("lib") appendPathSeq("io")
	docSlot("ioPath", "Returns the path of io installation. The default is $INSTALL_PREFIX/lib/io.")

	docSlot("getOptions(args)", "
	This primitive is used to get command line options similar to Cs getopt().
	It returns a map in containing the left side of the argument, with the
	value of the right side. (The key will not contain
	the beginning dashes (--).
	<p>
	Example:
	<pre>
	options := System getOptions(args)
	options foreach(k, v,
	  if(v type == List type,
		v foreach(i, j, writeln(\"Got unnamed argument with value: \" .. j))
		continue
	  )
	  writeln(\"Got option: \" .. k .. \" with value: \" .. v)
	)
	</pre>
	")

	getOptions := method(arguments,
		opts := Map clone
		optname := Sequence clone
		optvalue := Sequence clone
		optsNoKey := List clone

		arguments foreach(i, arg,
			if(arg beginsWithSeq("--") isNil,
				optsNoKey append(arg)
				continue
			)

			if(arg containsSeq("=")) then(
				optname := arg clone asMutable
				optname clipAfterStartOfSeq("=")
				optname clipBeforeEndOfSeq("--")
				optvalue := arg clone asMutable
				optvalue clipBeforeEndOfSeq("=")
			) else(
				optname := arg clone asMutable
				optname clipBeforeEndOfSeq("--")
				optvalue = ""
			)
			opts atPut(optname, optvalue)
		)

		if(optsNoKey last != nil, opts atPut("", optsNoKey))
		opts
	)

)
