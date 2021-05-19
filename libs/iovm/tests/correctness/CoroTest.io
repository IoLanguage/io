CoroTest := UnitTest clone do(
    testRecentInChain := method(
        o := Object clone
        o s := Sequence clone
        o l := method(
            j := 1
            loop(
                s appendSeq("a", j asString, ".")
                if(j%2==0, pause)
                j = j + 1
            )
        )

        run := method(
            o @@l

            for(i,1,4,
                yield
                o s appendSeq("b", i asString, ".")
                if(i==2, o actorCoroutine recentInChain resumeLater)
            )
        )

        run
        assertEquals("a1.a2.b1.b2.a3.a4.b3.b4.", o s asString)
    )
)
