
doFile("_ioCode/Exception.io")

ee := try(
	e := try(foo) 

	e catch(Exception, 
		writeln("caught: ", e error)
		//Exception raise("???")
		e showStack
		System exit
		e pass
	)

	write("done\n")
)

ee catch(Exception, 
	  writeln("caught2: ", ee error)
)