Event do(
	//doc Event category Networking
	descriptorId ::= -1
	eventType ::= nil
	timeout ::= 10
	coro ::= nil // internal
	EV_TIMER ::= 0
	//debugWriteln := getSlot("writeln")
	
	eventTypeName := method(
		if(eventType == EV_READ,   return "EV_READ")
		if(eventType == EV_WRITE,  return "EV_WRITE")
		if(eventType == EV_SIGNAL, return "EV_SIGNAL")
		if(eventType == EV_TIMER,  return "EV_TIMER")
		"?"
	)

	/*doc Event handleEvent(timeout) 
	
	*/
	handleEvent := method(isTimeout,
		//writeln("Event ", eventTypeName, " handleEvent(", isTimeout, ")")
		//debugWriteln(coro label, " resuming - got ", eventTypeName)
		self isTimeout := isTimeout
		if(coro,
			tmpCoro := coro
			//debugWriteln("Event handleEvent - resuming ", coro label)
			setCoro(nil)
			tmpCoro resumeLater
			//yield
		)
	)

	/*doc Event waitOnOrExcept(timeout) 
	*/
	waitOn := method(t,
		if(t, timeout = t)

		if(coro, return(Error with("Already waiting on this event")))
		coro = Scheduler currentCoroutine
		//writeln(coro label, " ", eventTypeName, " waitOn(", t, ") - pausing")
		EventManager addEvent(self, descriptorId, eventType, timeout) ifError(e, coro = nil; return(e))
		coro pause
		debugWriteln(Scheduler currentCoroutine label, " Event waitOn(", t, ") - resumed")
		if(isTimeout, Error with("Timeout"), self)
	)

	//doc Event waitOnOrExcept(timeout) Same as waitOn() but an exception is raised if a timeout occurs. Returns self.
	waitOnOrExcept := method(t,
		waitOn(t)
		isTimeout ifTrue(Exception raise("timeout"))
		self
	)
	
	timeoutNow := method(
		EventManager resetEventTimeout(self, 0)
		self
	)
	
	resetTimeout := method(
		EventManager resetEventTimeout(self, timeout)
		self
	)
)

ReadEvent := Event clone setEventType(Event EV_READ) do(
	//metadoc ReadEvent category Networking
	//metadoc ReadEvent description Object for read events.
	nil
)

WriteEvent  := Event clone setEventType(Event EV_WRITE) do(
	//metadoc WriteEvent category Networking
	//metadoc WriteEvent description Object for write events.
	nil
)

SignalEvent := Event clone setEventType(Event EV_SIGNAL) do(
	//metadoc SignalEvent category Networking
	//metadoc SignalEvent description Object for signal events.
	nil
)

TimerEvent  := Event clone setEventType(Event EV_TIMER) do(
	//metadoc TimerEvent category Networking
	//metadoc TimerEvent description Object for timer events.
	nil
)

Object wait := method(t, TimerEvent clone setTimeout(t) waitOn)

EventManager do(
	//metadoc EventManager category Networking
	/*metadoc EventManager description 
	Object for libevent (kqueue/epoll/poll/select) library. 
	Usefull for getting notifications for descriptor (a socket or file) events.
	Events include read (the descriptor has unread data or timeout) and write (the descriptor wrote some data or timeout).
	Also, timer and signal events are supported.
	*/
	isRunning ::= false
	coro ::= nil
	
	realAddEvent := getSlot("addEvent")

	/*doc EventManager addEvent(event, descriptor, eventType, timeout) 
	*/
	addEvent := method(e, descriptorId, eventType, timeout,
		//writeln("EventManager addEvent " .. e eventTypeName .. " - begin")
		r := self realAddEvent(e, descriptorId, eventType, timeout)
		r returnIfError
		resumeIfNeeded
		//debugWriteln("EventManager addEvent " .. e eventTypeName .. " - done")
		r
	)
	
	resumeIfNeeded := method(
		if(coro, coro resumeLater, self coro := coroFor(run); coro setLabel("EventManager"); coro resumeLater)	
	)

	//doc EventManager run Runs the EventManger loop. Does not return.
	run := method(
		//Scheduler currentCoroutine setLabel("EventManager")
		debugWriteln("EventManager run")
		loop(
			setIsRunning(true)
			while(hasActiveEvents,
				//debugWriteln("EventManager run - listening")
				if(Coroutine yieldingCoros first, listen, listenUntilEvent) ifError(e, 
					Exception raise("Unrecoverable Error in EventManager: " .. e description))
				yield
			)
			//debugWriteln("EventManager run - no active events")
			setIsRunning(false)
			coro pause
			//debugWriteln("EventManager run - resuming")
		)
	)
)

Scheduler currentCoroutine setLabel("main")
EventManager setListenTimeout(.01)

EvConnection do(
	eventManager ::= EventManager
	address ::= ""
	port ::= 80
	
	newRequest := method(
		EvRequest clone setConnection(self)
	)
)

EvRequest do(
	requestHeaders := Map clone
	requestHeaders atPut("User-Agent", "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) AppleWebKit/312.8 (KHTML, like Gecko) Safari/312.6)")
	requestHeaders atPut("Connection", "close")
	requestHeaders atPut("Accept", "*/*")
	
	init := method(
		self requestHeaders := requestHeaders clone		
	)

	connection ::= nil
	requestType ::= "GET"
	uri ::= "/index.html"

	send := method(
		r requestHeaders atPut("Host", connection address, connection port)
		self waitingCoro := Coroutine currentCoroutine
		asyncSend
		EventManager resumeIfNeeded
		yield
		waitingCoro pause
	)

	didFinish := method(
		waitingCoro resumeLater
	)
)
