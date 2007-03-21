WordyBuilder := Object clone do(
    newSlot("children")
    newSlot("_value")
    newSlot("depth", 0)
    newSlot("messageName")
    newSlot("name")

    init := method(
        children = Map clone
    )

    altMessageName := method("_" .. name)

    get := method(name,
        if(children hasKey(name) not,
            children atPut(name, WordyBuilder clone setDepth(depth + 1) setMessageName(
                    if(messageName, messageName .. name asCapitalized, name)
                ) setName(name)
            )
        )
        children at(name)
    )

    isEmpty := method(
        children size == 0
    )

    hasChildren := method(
        isEmpty not
    )

    value := method(
        self getSlot("_value")
    )

    setValue := method(x,
        self setSlot("_value", getSlot("x"))
        self
    )

    hasValue := method(
        self getSlot("_value") != nil
    )

    handlerMethod := method(target,
        #writeln("name: ", messageName, ", depth: ", depth, ", ", if(hasValue, "has", "no"), " value, ", if(hasChildren, "has", "no"), " children")
        if(hasValue,
            if(hasChildren,
                if(depth == 1,
                    target setSlot(altMessageName, value)
                    buildFirstWordWithValueHandler(target)
                ,
                    buildInnerWordWithValueHandler(target)
                )
            ,
                if(depth == 1,
                    value
                ,
                    buildLeafHandler 
                )
            )
        ,
            if(depth == 1,
                buildFirstWordWithoutValueHandler(target)
            ,
                buildInnerWordWithoutValueHandler(target)
            )
        )
    )

    buildWordyScope := method(target,
        result := Object clone
        children foreach(name, builder,
            result setSlot(name, builder handlerMethod(target))
            if(builder hasValue,
                target setSlot(builder messageName, builder value)
            )
        )
        result
    )
    
    buildFirstWordWithoutValueHandler := method(target,
        f := method(
            wordyScope := call activated wordyScope clone
            wordyScope target := self
            wordyScope
        ) clone
        getSlot("f") wordyScope := buildWordyScope(target)
        getSlot("f")
    )

    buildFirstWordWithValueHandler := method(target,
        f := method(
            if(call hasArgs,
                call delegateToMethod(self, "_name")
            ,
                wordyScope := call activated wordyScope clone
                wordyScope target := self
                wordyScope
            )
        ) clone
        getSlot("f") message argAt(1) attachedMessage argAt(1) setCachedResult(altMessageName)
        getSlot("f") wordyScope := buildWordyScope(target)
        getSlot("f")
    )

    buildInnerWordWithValueHandler := method(target,
        f := method(
            if(call hasArgs,
                call delegateToMethod(target, "mashedTogetherWords")
            ,
                self setProto(call activated wordyScope)
            )
        ) clone
        getSlot("f") message argAt(1) attachedMessage argAt(1) setCachedResult(messageName)
        getSlot("f") wordyScope := buildWordyScope(target)
        getSlot("f")
    )

    buildInnerWordWithoutValueHandler := method(target,
        f := method(
            self setProto(call activated wordyScope)
        ) clone
        getSlot("f") wordyScope := buildWordyScope(target)
        getSlot("f")
    )

    buildLeafHandler := method(
        f := method(
            call delegateToMethod(target, "mashedTogetherWords")
        ) clone
        getSlot("f") message attachedMessage argAt(1) setCachedResult(messageName)
        getSlot("f")
    )
)
