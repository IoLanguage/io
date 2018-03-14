/*
 * Generate an IoFooInit.c file for the Foo addon.
 * Most of the logic was pulled from the old AddonBuilder.io so
 * it's not a great solution, but it works.
 */

Addon := Object clone do(
	platform := System platform split at(0) asLowercase
	onWindows := platform == "windows" or platform == "mingw"
	isStatic := false

	with := method(dir,
		r := self clone
		r setDirectory(dir)
		r
	)

	// Force the user to clone the Addon.
	init := method(
		self directory ::= Directory clone
	)

	codeFolder := method(str, directory directoryNamed(str))
	ioFiles := inlineMethod(codeFolder("io") filesWithExtension("io"))
	initFileName := inlineMethod("source/Io#{directory name}Init.c" interpolate)

	generateInit := method(
		if(onWindows not and directory directoryNamed("source") filesWithExtension("m") size != 0, return)
		initFile := directory fileNamed(initFileName) remove create open
		initFile write("#include \"IoState.h\"\n")
		initFile write("#include \"IoObject.h\"\n\n")

		sourceFiles := directory directoryNamed("source") files
		iocFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".c")) and(f name containsSeq("Init") not) and(f name containsSeq("_") not))
		iocppFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".cpp")) and(f name containsSeq("Init") not) and(f name containsSeq("_") not))

		iocFiles appendSeq(iocppFiles)
		extraFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".c")) and(f name containsSeq("Init") not) and(f name containsSeq("_")))

		orderedFiles := List clone appendSeq(iocFiles)

		iocFiles foreach(f,
			d := f open readLines detect(line, line containsSeq("docDependsOn"))
			f close

			if(d,
				prerequisitName := "Io" .. d afterSeq("(\"") beforeSeq("\")") .. ".c"
				prerequisit := orderedFiles detect(of, of name == prerequisitName )
				orderedFiles remove(f)
				orderedFiles insertAfter(f, prerequisit)
			)
		)

		iocFiles = orderedFiles

		iocFiles foreach(f,
			initFile write("IoObject *" .. f name fileName .. "_proto(void *state);\n")
		)

		extraFiles foreach(f,
			initFile write("void " .. f name fileName .. "Init(void *context);\n")
		)

		if (onWindows,
			initFile write("__declspec(dllexport)\n")
		)
		initFile write("\nvoid Io" .. directory name .. "Init(IoObject *context)\n")
		initFile write("{\n")
		if(iocFiles size > 0,
			initFile write("\tIoState *self = IoObject_state((IoObject *)context);\n\n")
		)

		iocFiles foreach(f,
			initFile write("\tIoObject_setSlot_to_(context, SIOSYMBOL(\"" .. f name fileName asMutable removePrefix("Io") .. "\"), " .. f name fileName .. "_proto(self));\n\n")
		)

		extraFiles foreach(f,
			initFile write("\t" .. f name fileName .. "Init(context);\n")
		)

		if(codeFolder("io") and isStatic,
			ioFiles foreach(f, initFile write(codeForIoFile(f)))
		)

		initFile write("}\n")
		initFile close
	)
)

if(System args size > 2,
	Addon with(Directory with("#{System args at(1)}/#{System args at(2)}" interpolate)) generateInit
,
	"#{System args at(0)}: Incorrect number of arguments. Need a path to addons dir and name of the addon." interpolate println
)
