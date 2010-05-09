Logging := Object clone do(
    separator := "::"
    loggers := Map clone

    getLogger := method(prefix, handler,
        loggers hasKey(prefix) ifTrue(
            return loggers at(prefix)
        )
        if(call message argAt(1) != nil,
            l := Logger clone
            l setPrefix(prefix)
            l addHandler(handler)
            loggers atPut(prefix, l)
            return l
        ) else(
            l := Logger clone
            l setPrefix(prefix)
            // use parent's handlers if available
            l addHandler(ScreenHandler clone)
            loggers atPut(prefix, l)
            return l
        )
    )
)
