
Event do(
	docCategory("Networking")
    newSlot("descriptorId", -1)
    newSlot("eventType", nil)
    newSlot("timeout", 10)
    newSlot("coro", nil) // internal
    setSlot("EV_TIMER", 0)

    debugWriteln := nil
    debugOff := method(self debugWriteln := nil)
    debugOn := method(self debugWriteln := getSlot("writeln"))
    //debugOn
    
    eventTypeName := method(
        if(eventType == EV_READ,   return "EV_READ")
        if(eventType == EV_WRITE,  return "EV_WRITE")
        if(eventType == EV_SIGNAL, return "EV_SIGNAL")
        if(eventType == EV_TIMER,  return "EV_TIMER")
        "?"  
    )
    
    handleEvent := method(isTimeout,
        debugWriteln("Event ", eventTypeName, " handleEvent(", isTimeout, ")")
        //debugWriteln(coro label, " resuming - got ", eventTypeName)
        self isTimeout := isTimeout
        if(coro,
            tmpCoro := coro
        	debugWriteln("Event handleEvent - resuming ", coro label)
            setCoro(nil)
            tmpCoro resumeLater
            yield
        )
    )
    
    waitOn := method(t,
        if(t, timeout = t)

        if(coro, Exception raise("already waiting on this event"))
        coro = Scheduler currentCoroutine
        debugWriteln(coro label, " Event waitOn(", t, ") - pausing")

        EventManager addEvent(self, descriptorId, eventType, timeout) ifFalse(coro = nil; return false)
        coro pause
		debugWriteln(Scheduler currentCoroutine label, " Event waitOn(", t, ") - resumed")
       isTimeout == false
    )
    
    waitOnOrExcept := method(t,
        waitOn(t)
        isTimeout ifTrue(Exception raise("timeout"))
    )
)

ReadEvent := Event clone setEventType(Event EV_READ) do(
	docCategory("Networking")
)

WriteEvent  := Event clone setEventType(Event EV_WRITE) do(
	docCategory("Networking")
)

SignalEvent := Event clone setEventType(Event EV_SIGNAL) do(
	docCategory("Networking")
)

TimerEvent  := Event clone setEventType(Event EV_TIMER) do(
	docCategory("Networking")
)

Object wait := method(t, TimerEvent clone setTimeout(t) waitOn)

EventManager do(
	docCategory("Networking")
    newSlot("isRunning", false)
    newSlot("coro", nil)
    debugWriteln := nil
    realAddEvent := getSlot("addEvent")
    
    addEvent := method(e, descriptorId, eventType, timeout,
        debugWriteln("EventManager addEvent - begin")
        r := self realAddEvent(e, descriptorId, eventType, timeout)
        if(coro, coro resumeLater, self coro := coroFor(run); coro setLabel("EventManager"); coro resumeLater) 
        debugWriteln("EventManager addEvent - done")
        //Coroutine showYieldingCoros
        r
    )

    run := method(
		//Scheduler currentCoroutine setLabel("EventManager")       
		debugWriteln("EventManager run")
		loop(
			setIsRunning(true)
			while(hasActiveEvents,
				debugWriteln("EventManager run - listening")
				if(Coroutine yieldingCoros first, listen, listenUntilEvent)
				yield
			)
			debugWriteln("EventManager run - no active events")
			setIsRunning(false)
			coro pause
			debugWriteln("EventManager run - resuming")
		)
    )
)

Scheduler currentCoroutine setLabel("main")       
EventManager setListenTimeout(0.03)
