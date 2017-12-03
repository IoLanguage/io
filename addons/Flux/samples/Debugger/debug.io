#!/usr/bin/env io

Flux

Runner := Object clone do(  
  run := method(msg,
    self turnOnMessageDebugging
    Lobby doString(msg)
  )
)

Debugger do(

  breakpoints := List clone
  continueToBreakpoint := nil

  vmWillSendMessage = method(actor, msg, target, context,
    self actor := actor
    self msg := msg
    self target := target
    self context := context
    writeln(msg asString)

    if (continueToBreakpoint,
      if (breakpoints contains(msg),
        continueToBreakpoint = nil
        breakpoints remove(msg)
      ,
        actor ?resume
        return
      )
    )

    if (context and context hasSlot("thisBlock"), 
      blkmsg := context getSlot("thisBlock") message
      if (blkmsg == Runner getSlot("run") message,
        actor ?resume
        return
      )
      if (blkmsg,
        view msg setMessage(blkmsg) redisplay
        if (msg ?ghoster,
          view msg setCursor(msg)
          if (msg nextMessage, self nextLine := msg nextMessage)
        ,
          actor ?resume
        )
      ,
        actor ?resume
      )
    ,
      actor ?resume
    )
  )

  resume := method(
    actor ?resume
  )

  next := method(
    follow := msg attachedMessage
    if (follow == nil,
      follow = msg nextMessage
    )
    if (follow,
      breakpoints append(follow)
    ,
      if (?nextLine, breakpoints append(nextLine))
    )
    view msg setCursor(nil)
    continueToBreakpoint = 1
    actor ?resume
  )

  returnToCaller := method(
    if (context and context hasSlot("thisMessage"),
      follow := context thisMessage attachedMessage
      if (follow == nil,
        follow = context thisMessage nextMessage
      )
      if (follow,
        breakpoints append(follow)
      )
      view msg setCursor(nil)
      continueToBreakpoint = 1
      actor ?resume
    )
  )

  view := View clone do(
    size set(Screen width, Screen height, 0)
    step := Button clone setTitle("Step")
    next := Button clone setTitle("Next")
    returner := Button clone setTitle("Return")
    msg := MessageView clone setMessage("1 + 2" asMessage)
    msg size set(Screen width, Screen height - step height, 0)
    step position set(0, msg height, 0)
    next position set(step width, msg height, 0)
    returner position set(step width + next width, msg height, 0)
    step actionTarget := Debugger
    step action := "resume"
    next actionTarget := Debugger
    next action := "next"
    returner actionTarget := Debugger
    returner action := "returnToCaller"
    addSubview(msg)
    addSubview(next)
    addSubview(returner)
    addSubview(step)
  )

  debug := method(msg,
    Runner @@run(msg)
  )
)


doFile("foo.io")


Screen addSubview(Debugger view)
Screen @@open

Debugger debug("Foo bar")

