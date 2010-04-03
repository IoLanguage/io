#!/usr/bin/env io

if(System args size > 1,
	# Run specific tests.
	UnitTest verbose := getSlot("writeln")

    System args slice(1) foreach(name,
        try(
            if(name endsWithSeq(".io"),
                doFile(name) run
            ,
                doString(name) run
            )
        ) ?showStack
	)
,
	# Run all tests.
	TestSuite with(System launchPath) run
)