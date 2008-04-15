#!/usr/bin/env io

//Message useIoShuffle
//Collector debugOn

if(System args size > 1,
	# Run specific tests
	UnitTest verbose := getSlot("writeln")

	time := Date clone cpuSecondsToRun(
		System args slice(1) foreach(name,
			writeln(name, ":")
			try(
				if(name endsWithSeq(".io"),
					doFile(name) run
				,
					doString(name) run
				)
			) ?showStack
			writeln
		)
	)
	writeln("time: ", time, " seconds")
	System exit(0)
,
	# Run all tests
	test := method(TestSuite clone setPath(System launchPath) run)

	time := Date clone cpuSecondsToRun(r := test)
	writeln("time: ", time, " seconds")
	System exit(r)
)

/*
allocsPerMark
marksPerSweep
sweepsPerGeneration
maxAllocatedBytes (millions)
time to run
*/

writeln("mpa	as	mb	time	mbt	gc")

lastTimeUsed := 0

//aaa := method(1.1 + 2.2)
//test := method(10000 repeat(aaa))

list(1.01, 1.05, 1.1, 1.2, 1.5, 1.7, 2, 4) foreach(as,
list( 0.01, .1, 1, 2, 4, 16) foreach(mpa,

	Collector setMarksPerAlloc(mpa)
	Collector setAllocatedStep(as)

	time := Date clone cpuSecondsToRun(test)
	mb := (Collector maxAllocatedBytes/1000000) asString(0,2)

	writeln(Collector marksPerAlloc asString(0,2), "\t",
		Collector allocatedStep asString(0,2), "\t",
		mb, "\t",
		time asString(0,2) , "\t",
		(mb asNumber * time  ) asString(0,2), "\t",
		(100*(Collector timeUsed - lastTimeUsed)/time) asString(2, 1), "%")

	lastTimeUsed = Collector timeUsed

	Collector collect
	Collector resetMaxAllocatedBytes

)
"" println
)

/*
writeln("mpa	mps	spg	mb	time	mbt")

list(16, 64, 128) foreach(spg,
list(1, 10, 100) foreach(mpa,
list(100, 10000) foreach(mps,

	Collector setMarksPerAlloc(mpa)
	Collector setMarksPerSweep(mps)
	Collector setSweepsPerGeneration(spg)

	time := Date clone cpuSecondsToRun(r := TestSuite clone setPath(System launchPath) run)
	mb := (Collector maxAllocatedBytes/1000000) asString(0,1)

	writeln(Collector marksPerAlloc asString(0,1), "\t", Collector marksPerSweep, "\t", Collector sweepsPerGeneration, "\t", mb, "\t", time asString(0,1) , "\t", (mb asNumber * time) asString(0,1) )

	30 repeat(Collector collect)
	Collector resetMaxAllocatedBytes

)
"" println
)
"" println
)

System exit(r)
*/
