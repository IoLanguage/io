REBOL [
    Title: "Speed benchmark for comparison agains Io"
    Author: ["Jaime Vargas" jev@mac.com]
    Description: "Derived from speed.io by Steve Dekorte"
]

oneMillion: 1'000'000

Tester: make object! [

    results: make block! []

    foo: func [][1]
    
    t1: none
    x: none

    as-seconds: func[t][t/second + (t/minute * 60) + (t/hour * 3600)]
        
    beginTimer: does [t1: now/time/precise]
    
    endTimer: func [s /local mps][
        mps: 1 / (as-seconds now/time/precise - t1)
        print ["Rebol" s ":=" mps]
    ]
    
    testSlot: func[][
        beginTimer
        loop oneMillion / 8 [
            x x x x
            x x x x
        ]
        endTimer "slotAccesses      "
    ]
    
    testSetSlot: func[][
        beginTimer
        loop oneMillion / 8 [
            x: 1 x: 2 x: 3 x: 4
            x: 1 x: 2 x: 3 x: 4
        ]
        endTimer "slotSets          "
    ]
    
    testBlock: does [
        beginTimer
        loop oneMillion / 8 [
            foo foo foo foo
            foo foo foo foo
        ]
        endTimer "blockActivations  "
    ]
    
    testInstantiations: does [
        beginTimer
        loop oneMillion / 8 [
            make Tester [] make Tester [] make Tester [] make Tester []
            make Tester [] make Tester [] make Tester [] make Tester []
        ]
        endTimer "instantiations    "
    ]
    
    testLocals: func[/local v][
        beginTimer
        v: 1
        loop oneMillion / 8 [
            v v v v
            v v v v
        ]
        endTimer "localAccesses     "
    ]
    
    testSetLocals: func[/local v][
        beginTimer
        v: "1"
        loop oneMillion / 8 [
            v: 1 v: 2 v: 3 v: 4
            v: 1 v: 2 v: 3 v: 4
        ]
        endTimer "localSets         "
    ]
    
    test: does [
        print newline
        testLocals
        testSetLocals
        print newline
        testSlot
        testSetSlot
        print newline
        testBlock
        testInstantiations
        print newline
        print ["Rebol version :=" system/version]
        print "// values in millions per second"
    ]
]

Tester/test

halt
