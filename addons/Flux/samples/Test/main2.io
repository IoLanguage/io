#!/usr/bin/env io

Flux

Controller := Object clone do(
	push := method(button,
		button setTitle("Please don't push this button again.")
	)
)

app := Application clone

app appDidStart := method(
    app setTitle("Test")
    b := Button clone
    b setTitle("Foo")
    b setWidth(300)
    b setActionTarget(Controller)
    b setAction("push")
    app mainWindow addSubview(b)
)

app run
