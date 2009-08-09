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
	
	setObject := method(obj, newSelectedSlot,
		_object = getSlot("obj")
		selectedSlotName = if(newSelectedSlot, newSelectedSlot, objectSlotNames first)
		object _proto := object proto
	)
	
	object := method(
		self getSlot("_object")
	)
	
	setObject(Lobby)

	show := method(
		objectSlotNames foreach(i, slotName,
			if (i > Curses height - 2, break)
			Curses move(0, i)
			s := if(slotName == selectedSlotName, ">", " ")
			Curses write(s .. slotName)
			Curses move(20, i)
			Curses write("" .. object getSlot(slotName) asSimpleString)
		)
		Curses move( 0, Curses height - 1)
		Curses write("<Up>/<Down> to navigat the slot names, <Right>/<Left> to navigate proto hierarchies, <ESC> to quit")
	)
	
	selectPreviousSlot := method(
		sn := objectSlotNames 
		index := sn indexOf(selectedSlotName)
		if (0 < index, setSelectedSlotName(sn at(index - 1)))
	)
	
	selectNextSlot := method(
		sn := objectSlotNames 
		index := sn indexOf(selectedSlotName)
		if ((sn size - 1 > index), setSelectedSlotName(sn at(index + 1)))
	)
	
	up   := method(selectPreviousSlot)
	down := method(selectNextSlot)
	
	next := method(
		objectStack push(list(object, selectedSlotName))
		setObject(object getSlot(selectedSlotName))
	)
	
	previous := method(
		obj := objectStack pop
		if(obj, setObject(obj first, obj second))
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
		if(c == 27, Lobby exit)
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
