ContextScope := Object clone do(
    newSlot("name")
    newSlot("scope")
    newSlot("setupBody")
    newSlot("expectedException", nil)
    
    init := method(
        scope = Object clone
        scope should := should clone
        scope should setContext(self)
        scope should not setContext(self)
    )

    setup := method(
        setupBody = call argAt(0)
    )

    specify := method(name,
        specificationScope := scope clone
        if(setupBody,
            setupError := try(specificationScope doMessage(setupBody, specificationScope))
            if(setupError,
                writeln("Unexpected Exception while setting up: ", self name, " ", name, ".")
                setupError showStack
                return
            )
        )
        e := try(specificationScope doMessage(call argAt(1), specificationScope))
        if(expectedException,
            if(e,
                if(e isKindOf(expectedException) not,
                    writeln("Unexpected Exception: ", self name, " ", name, ".")
                    e showStack
                )
            ,
                writeln("Expected Exception: ", self name, " ", name, ".")
                writeln
                writeln("  Expected a '", expectedException type, "' exception.")
                writeln
            )
        ,
            if(e,
                writeln("Unexpected Exception: ", self name, " ", name, ".")
                e showStack
            )
        )
        setExpectedException(nil)
    )

    should := Object clone do(
        newSlot("context")
        raise := method(expectedException,
            context setExpectedException(expectedException)
        )
    )

    should not := Object clone do(
        newSlot("context")
        raise := method
    )
)
