#!/usr/bin/env io

Curses do(
	begin
	clear
)

Lobby exit := method(Curses end; System exit)

Inspector := Object clone do(
	_object := Lobby
	offset := 0
	selectedSlotName ::= ""
	objectStack := List clone
	
	objectSlotNames := method(object slotNames sort)
	
	setObject := method(obj,
		_object = getSlot("obj")
		selectedSlotName = objectSlotNames first
		object _proto := object proto
	)
	
	object := method(
		self getSlot("_object")
	)
	
	setObject(Lobby)

	show := method(
		objectSlotNames foreach(i, slotName,
			if (i > Curses height - 1, break) 
			Curses move(0, i)
			s := if(slotName == selectedSlotName, ">", " ")
			Curses write(s .. slotName)
			Curses move(20, i)
			Curses write(object getSlot(slotName) asSimpleString)
		)
	)
	
	selectPreviousSlot := method(
		sn := objectSlotNames 
		setSelectedSlotName(sn at(sn indexOf(selectedSlotName) - 1))		
	)
	
	selectNextSlot := method(
		sn := objectSlotNames 
		setSelectedSlotName(sn at(sn indexOf(selectedSlotName) + 1))
	)
	
	up   := method(selectPreviousSlot)
	down := method(selectNextSlot)
	
	next := method(
		objectStack push(object)
		setObject(object getSlot(selectedSlotName))
	)
	
	previous := method(
		obj := objectStack pop
		if(obj, setObject(obj))
	)
)

loop(
	Inspector show
	
	loop(
		c := Curses asyncReadCharacter
		if(c, break, yield)
	)
	
	e := try(
		if(c == 259, Inspector up)
		if(c == 258, Inspector down)
		if(c == 261, Inspector next)
		if(c == 260, Inspector previous)
		if(c == 27, Curses end; System exit)
	)
	
	e catch(Exception,
		Curses clear
		Curses end
		write(e coroutine backTraceString)
		write("\n---\n")
		System exit
	)
	
	Curses clear
)

Curses end
