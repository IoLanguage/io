Logger := Object clone do(
    logLevel := 0
    handlers := list clone
    logLevels := Map clone
    format := "#{mname} #{tstamp} #{fname}:#{fline} #{text}"
    prefix := "root"
    parent := nil
    propagate := true

    init := method(
        handlers = list clone
        logLevel = 0
        logLevels = Map clone
        format := "#{mname} #{tstamp} #{fname}:#{fline} #{text}"
        prefix := "root"
        addLogLevel("trace", 0)
        addLogLevel("debug", 10)
        addLogLevel("info", 20)
        addLogLevel("warn", 30)
        addLogLevel("error", 40)
        addLogLevel("fatal", 50)
    )

    log := method(mname, text, fline, fname, tstamp,
        handlers foreach(handler,
            format interpolate println
        )
    )

    setPrefix := method(pref,
        prefix := pref
    )

    addLogLevel := method(idname, level,
        logLevels atPut(idname, level)
        self setSlot(idname,
            method(text,
                if(logLevel <= logLevels at(call message name),
                    fline := call message lineNumber
                    mname := call message name asUppercase
                    fname := call message label
                    tstamp := Date asString("%Y-%m-%d %H:%M:%S")
                    log(mname, text, fline, fname, tstamp)
                )
            )
        )
    )

    addHandler := method(lhandler,
        handlers append(lhandler)
    )
)
