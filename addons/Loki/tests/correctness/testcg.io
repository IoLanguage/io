
Directory setCurrentWorkingDirectory("Loki")
doFile("Loki.io")
Directory setCurrentWorkingDirectory("..")

IoVM Utils := Object clone

Loki Implementation linkToAs( Utils, "rdtsc_" ) with( _self state   numberWithDouble(double rdtsc asFloat64) )

Utils rdtsc := Utils getSlot("rdtsc_") clone

Loki Implementation linkToAs( Sequence, "getByteStringPointer" ) with( 
	_self state numberWithDouble( (_self data.ptr castAs(ByteArray) bytes asFloat64 ) )
)




Chrono := Object clone do(
	timer ::= 0
	
//	init := method( self timer := 0 )
	
	measure := method(
		c := Utils rdtsc
		r := call evalArgAt(0)
	//	timer println
		timer = timer + (Utils rdtsc-c)
	//	timer println
		r
	)
	asString := method( "#{timer / (3* (10 **(9)))} sec" interpolate )
	reset := method( timer = 0 )
)

/*
chrono := method(
	c := Utils rdtsc
	call evalArgAt(0)
	c = Utils rdtsc-c
	"#{c / (3* (10 **(9)))} sec" interpolate
)*/


"Done."




























































