
TupleTest := UnitTest clone do(
    setUp := method(
        super(setUp)
        self exampleTuple := Tuple clone set(1,"test",3,4,1)

    )

    testClone := method(
        assertNotSame(Tuple, Tuple clone)
        assertEquals(5, exampleTuple size)
        clonedTuple := exampleTuple clone
        assertNotSame(clonedTuple, exampleTuple)
        assertEquals(5, clonedTuple size)
        assertEquals(exampleTuple at(1), clonedTuple at(1))
        assertEquals(exampleTuple at(0), clonedTuple at(0))
    )

    testAppend := method(
        a := Tuple clone set(1,2,3)
        b := Tuple clone set(4,5,6)
        c := a+b
        assertNotSame(c,a)
        assertNotSame(c,b)
        assertEquals(c,Tuple clone set(1,2,3,4,5,6))

        //you can't concat a list and a tuple
        d := list(1,2)
        assertRaisesException(a + d)
    )

    testAt :=  method(
        assertRaisesException(exampleTuple at())
        assertRaisesException(exampleTuple at(nil))
        assertRaisesException(exampleTuple at("test"))
        assertEquals(1, exampleTuple at(0))
        assertEquals("test", exampleTuple at(1))
        assertNil(exampleTuple at(5))
        assertNil(exampleTuple at(10))
    )

    testsize := method(
        a := Tuple clone set(1)
        b := Tuple clone set(1,2,3)
        assertEquals(a size,1)
        assertEquals(b size,3)
    )

    testToList := method(
        a := Tuple clone set("test",1,23)
        b := a toList
        assertEquals(b at(0), "test")
        assertEquals(b at(1),1)
        assertEquals(b at(2),23)
    )

    testItemChange := method(
        a := list(1,2,3)
        b := Tuple clone set(1,a)
        assertEquals(b at(1),a)
        assertSame(b at(1),a)
        a append(4)
        assertEquals(b at(1),a)
        assertSame(b at(1),a)
    )

    testEmpy := method(
        a := Tuple clone set(1,2,3)
        a empty
        assertEquals(0, a size)
    )


)