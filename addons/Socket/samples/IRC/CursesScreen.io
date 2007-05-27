#!/usr/bin/env io


CursesView := Object clone do(
    buffer := Buffer clone
    position := Point clone
    size := Point clone
    
    init := method(
		resend
		buffer = buffer clone
		position = position clone
		size = size clone
    )
    
    newSlot("superview", nil)
    redisplay := method(superview ?redisplay)
    
    draw := method(
		move(0,0)
		show(buffer asString)
    	superview size = 1000
    )
    
    directKey := method(k,
		if (k == 4, self ?controlDKey; return)
		if (k == 13, self returnKey; return)
		if (k == 258, self ?downArrowKey; return)
		if (k == 259, self ?upArrowKey; return)
		if (k == 260, self ?leftArrowKey; return)
		if (k == 261, self ?rightArrowKey; return)
		if (k == 330, self ?delKey; return)
		if (k == 127, self ?deleteKey; return)
		if (k == 27, self ?escapeKey; return)
		key(k)
    )
    
    key := method(k,
		buffer append(k asCharacter)
		redisplay
    )
    
    screenWidth  := method(Curses width)
    screenHeight := method(Curses height)
    move := method(x, y, Curses move(Curses height - y , x))
    show := method(s, Curses print(s))
    
)

// ----------------------------------------

CursesScreen := Object clone do(
    setup := method(
		Curses init
		Curses noEcho
		Curses nodelay(1) // asynchronous input
		Curses clear
		Curses refresh
    )
    
    subviews := List clone
    addSubview := method(v, 
		subviews append(v)
		v setSuperview(self)
		if (firstResponder == nil, setFirstResponder(v))
    )
    
    removeSubview := method(v, subviews remove(v))
    
    display := method(
		Curses clear
		subviews foreach(view, view draw)
		if (debugMessage size > 0,
			Curses move(10, 10)
			Curses print(debugMessage)
		)
		Curses refresh
		needsDisplay = nil
		//size = size + 1
    )
    
    newSlot("firstResponder", nil)
    newSlot("needsDisplay", 1)
    redisplay := method(needsDisplay = 1)
    
    debugMessage := ""
    isRunning := nil
    eventLoop := method(
		isRunning = 1
		while(isRunning, 
			c := Curses getCh
			if (c, 
				firstResponder ?directKey(c)
				//debugMessage = c asString
				redisplay
			)
			if(needsDisplay, display)
	
			Curses refresh
			yield
			wait(.05)
		)
    )
    
    shutdown := method(
		Curses end
    )
)

//CursesScreen addSubview(CursesView clone)

// ----------------------------------------

CursesItem := Object clone do(
    newSlot("object", nil)
    setObject := method(v, object = getSlot("v"); self)
    newSlot("title", "")
    newSlot("isSelected", nil)
    
    subitems := method(
		items := List clone
		self getSlot("object") slotNames sort foreach(slotName,
			item := CursesItem clone 
			//item setTitle(slotName .. " " .. self getSlot("object") type .. "-" .. self getSlot("object") uniqueId) 
			item setTitle(slotName) 
			
			item setObject(self getSlot("object") getSlot(slotName))
			items append(item)
		)
		self subitems := items
		items
    )
    
    addSubitem  := method(item, subitems append(item))
    hasSubitems := method(subitems size > 0)
    select   := method(isSelected = 1)
    unselect := method(isSelected = nil)
)

CursesColumn := CursesView clone do(
	newSlot("delegate", nil)
	newSlot("selectedIndex", 0)
	size setWidth(30)
	draw := method(
		w := size width
		h := screenHeight
		move(1, h)
		show(delegate title .. " " .. delegate uniqueId)
		
		move(0, h-1)
		w repeat(show("-"))
		
		p := h-2
		if (delegate hasSubitems,
		    delegate subitems foreach(i, item,
			move(0, p)
			if (selectedIndex == i, Curses print("*"))
			move(1, p)
			show(item title)
			if (item hasSubitems, move(w-1, p); show(">"))
			p = p - 1
			if (p < 1, break)
		    )
		)
		
		for (p, 1, h, move(w, p); show("|"))
	)
	clipSelection := method(
	    selectedIndex = selectedIndex clip(0, delegate subitems size - 1) 
	    self selectedItem := item subitems at(selectedIndex)
	)
	
	upArrowKey := method(
	    selectedIndex = selectedIndex - 1
	    clipSelection
	    //CursesScreen debugMessage = "upArrowKey"
	)
	
    	downArrowKey := method(
	    selectedIndex = selectedIndex + 1
	    clipSelection
	    //CursesScreen debugMessage = "downArrowKey"
	)
	
	//selectedItem := method(item subitems at(selectedIndex))
	
	returnKey := method(
	    if (selectedItem, self setDelegate(selectedItem))
	    selectedIndex = 0
	)
)

item := CursesItem clone setTitle("Lobby") setObject(Lobby)
item subitems
CursesScreen addSubview(CursesColumn clone setDelegate(item))
