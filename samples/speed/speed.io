#!/usr/bin/env io

oneMillion := 1000000

results := List clone

Tester := Object clone do(
    foo := method(1)

    beginTimer := method(
	self t1 := Date clone now
    )
    
    endTimer := method(s, 
	mps := 1 / Date clone now secondsSince(t1)
	//write(t1 asString, " -> ", Date clone now asString , "\n")

	write("Io ", s, " := ", mps asString(0,2) , "\n")
    )
    
    testSlot := method(
	beginTimer
	self x := 1
	(oneMillion/8) repeat(
	   x; x; x; x; 
	   x; x; x; x; 
	)
	endTimer("slotAccesses      ")
    )

	// t := method(x := 1; loop(x = 1)); t

    testSetSlot := method(
	beginTimer
	self x := 1
	updateSlot := getSlot("updateSlot")
	(oneMillion/8) repeat(
	   x = 1; x = 1; x = 1; x = 1; 
	   x = 1; x = 1; x = 1; x = 1; 
	)
	endTimer("slotSets          ")
    )
    
    testBlock := method(
	beginTimer
	(oneMillion/8) repeat(
	   foo; foo; foo; foo; 
	   foo; foo; foo; foo; 
	)
	endTimer("blockActivations  ")
    )
    
    testInstantiations := method(
	beginTimer
	(oneMillion/8) repeat(
	    Tester clone; Tester clone; Tester clone; Tester clone;
	    Tester clone; Tester clone; Tester clone; Tester clone;
	)
	endTimer("instantiations    ")
    )

    testLocals := method(
	beginTimer
	v := 1
	(oneMillion/8) repeat(
	    v; v; v; v;  
	    v; v; v; v;
	)
	endTimer("localAccesses     ")
    )

    testSetLocals := method(
	beginTimer
	//updateSlot := getSlot("updateSlot")
	v := "1"
	(oneMillion/8) repeat(
	    v = 1; v = 2; v = 3; v = 4;  
	    v = 1; v = 2; v = 3; v = 4;  
	)
/*
	(oneMillion/8) repeat(
	    v = 1; v = 2; v = 3; v = 4;  
	    v = 1; v = 2; v = 3; v = 4;   
	)
*/
	endTimer("localSets         ")
    )

    testCFunc := method(
	beginTimer
	v := self getSlot("thisContext")
	(oneMillion/8) repeat(
	    v; v; v; v;  
	    v; v; v; v; 
	)
	endTimer("cfuncActivations  ")
    )

    test := method(
	write("\n")
	testLocals
	testSetLocals
	write("\n")
	testSlot
	testSetSlot
	write("\n")
	testBlock
	testInstantiations
	testCFunc
	write("Io version := ", System version)
	write("\n")
	write("// values in millions per second\n")
    )
)

//1000 repeat(Tester testSetLocals)
Tester test

