// aliases for interactive developpement

include := method(
	Directory setCurrentWorkingDirectory("Loki")
	Lobby doFile("Loki.io")
	Directory setCurrentWorkingDirectory("..")
)

cc := method( include; Lobby doFile("testcg.io") )
//xx := method( include; Lobby doFile("shootout/loki_nsieve.io") )
xx := method( include; Lobby doFile("shootout/loki_nsieve-bits.io") )
//ww := method( Lobby doFile("shootout/old_nsieve.io") )

/*
ww := method( Lobby do(

	include
	
	x86 Instruction do(
		_encode := getSlot("encode")
		encode := getSlot("encode2") 
		
	//	getSlot("_encode") println
		getSlot("encode") println
		
	)

	rac:= Chrono clone
	ec:= Chrono clone
	ac:= Chrono clone
	x := Chrono clone
	x measure(xx)
	x println
	rac println
	ec println
	ac println
	

	x86 Instruction do(
		encode2 := getSlot("encode")
		encode := getSlot("_encode") 
		
		getSlot("encode2") println
	//	getSlot("encode") println
		
	)
	
) 
nil
)*/

reset := method( Lobby doFile("main.io") )