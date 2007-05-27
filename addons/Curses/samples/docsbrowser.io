#!/usr/bin/env io

Curses do(
	begin
	clear
	readCharacter := method(
		loop(
			c := Curses asyncReadCharacter
			if(c, return c, Lobby yield)
		)
	)
)

Lobby exit := method(Curses end; System exit)

Outliner := Object clone do(
	_object := Lobby
	newSlot("selectedSlotName", "")

	init := method(
		resend
		self offset := 0
		self firstIndex := 0
	)
	
	with := method(v,
		self clone setObject(getSlot("v"))
	)
	
	objectSlotNames := method(object slotNames sort)
	
	setObject := method(obj,
		_object = getSlot("obj")
		selectedSlotName = objectSlotNames first
		//object _proto := object proto
		self
	)
	
	object := method(self getSlot("_object"))
	
	setObject(Protos)

	show := method(
		//if(self hasSlot("docs") and self docs getSlot("description"), showDocs; return)
		//if(object != IoVM, showDocs; return)

		objectSlotNames := objectSlotNames
		row := 0
		for(i, firstIndex, objectSlotNames size,
			slotName := objectSlotNames at(i)
			if (row > Curses height - 1, break) 
			Curses move(0, row)
			s := if(slotName == selectedSlotName, ">", " ")
			Curses write(s .. slotName)
			Curses move(20, row)
			//Curses write(object getSlot(slotName) asSimpleString)
			row = row + 1
		)
	)
	
	showDocs := method(
		Curses clear
		
		Curses move(0, 0)
		Curses write(object type)
		
		Curses move(0, 2)
		Curses write(object docs description)
	
	)
	
	selectPreviousSlot := method(
		sn := objectSlotNames 
		index := sn indexOf(selectedSlotName)
		if(index == 0, return)
		setSelectedSlotName(sn at(index - 1))		
		if(firstIndex != 0 and index <= firstIndex, firstIndex = firstIndex - 1)
	)
	
	selectNextSlot := method(
		sn := objectSlotNames 
		index := sn indexOf(selectedSlotName)
		if(index == objectSlotNames size -1, return)
		setSelectedSlotName(sn at(index + 1))
		if(index - firstIndex > Curses height - 2, firstIndex = firstIndex + 1)
	)

	selectedValue := method(object getSlot(selectedSlotName))
	outlinerForSelected := method(self with(object getSlot(selectedSlotName)))
)

Browser := Object clone do(
	outliners := List clone append(Outliner with(Lobby))
	outliner := method(outliners last)
	
	show := method(
		outliner show
	)
	
	keyDown := method(c,
		if(c == 259, up)
		if(c == 258, down)
		if(c == 261, next)
		if(c == 260, previous)
		if(c == 27, Curses end; System exit)
	)
	
	up   := method(outliner selectPreviousSlot)
	down := method(outliner selectNextSlot)
	next := method(outliners push(outliner outlinerForSelected))
	previous := method(if(outliners size > 1, outliners pop))
)

loop(
	e := try(
		Curses clear
		Browser show
		c := Curses readCharacter
		Browser keyDown(c)
	)
	
	e catch(Exception,
		Curses end
		writeln("done")
		writeln(e coroutine backTraceString)
		System exit
	)
)

