#!/usr/bin/env io

/* Io Calculator - 2004 Mike Austin */

Flux


Button textColor = Color clone set(1, 1, 1, 1)

calcApp := Application clone 
calcApp setTitle("Calc")

text := TextField clone do (position set(10, 210); size set(190, 32))
text font := TextField fonts system mono
text roundingSize := 5
text backgroundColor := Color clone set(.1,.1,.3,1)
text textColor set(1,1,1,1)

text setString(0 asString)
calcApp mainWindow contentView addSubview(text)
calcApp mainWindow backgroundColor set(0,0,0, .1)

Calculator := Object clone do(
  mem    := "0"
  operat := "+"
  entry  := 0

  pushButton := method(button,
    title := button title
  
    if(title == "C") then(
	  mem = "0"
	  text setString("0")
	  entry = 0
	) elseif(title == "+" or title == "-" or title == "*" or title == "/") then(
	  num := doString((mem .. " " .. operat .. " " .. text string) asString)
	  text setString(num asString)
	  mem = text string asString
	  operat = title
	  entry = 0
	) elseif(title == "=") then(
	  num := doString((mem .. " " .. operat .. " " .. text string) asString)
	  text setString(num asString)
	  mem = "0"
	  operat = "+"
	  entry = 0
	) else(
	  if(entry == 1) then(
	    text setString((text string .. title) asString)
	 ) else(
	    text setString(title)
	    entry = 1
	 )
   )
 )
)

CalcButton := Button clone do (
    size set(40, 40)
    setActionTarget(Calculator)
    setAction("pushButton")
    font := TextField fonts system mono
   	roundingSize := 40
    selectedColor := Color clone set(.3,.3,.3, 1)
    set := method (title, x, y,
        setTitle(title)
        if(x, position set(x, y))
        self
    )
)

calcApp mainWindow do(
    newButton := method(name, 
        b := CalcButton clone 
        b set(name)
        addSubview(b)
        b
    )
    
    z := newButton("0") 
    z position set(10, 10)
    b := newButton(".") placeRightOf(z)
    b := newButton("=") placeRightOf(b)
    b := newButton("/") placeRightOf(b)
    
    z := newButton("1") placeAbove(z)
    b := newButton("2") placeRightOf(z)
    b := newButton("3") placeRightOf(b)
    b := newButton("+") placeRightOf(b)
    
    z := newButton("4") placeAbove(z)
    b := newButton("5") placeRightOf(z)
    b := newButton("6") placeRightOf(b)
    b := newButton("-") placeRightOf(b)

    z := newButton("7") placeAbove(z)
    b := newButton("8") placeRightOf(z)
    b := newButton("9") placeRightOf(b)
    b := newButton("*") placeRightOf(b)
)

calcApp mainWindow resizeTo(210, 256)

calcApp run
