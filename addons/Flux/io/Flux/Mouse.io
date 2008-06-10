
Mouse := Object clone do(
    position := Point clone
    lastPosition := Point clone
    tmpPoint := Point clone // assumes sequential access...
    button := 0
    state := 0
      
    clickCount := 0
    lastClickTime := Date clone
    lastClickTime2 := Date clone
    currentTime := Date clone

    mouse := method(b, s, mx, my,
		button = b
		state = s
		//writeln("Mouse ", button, state)
		self motion(mx, my)

		if(s == 1, return)

		shortClick := currentTime now secondsSince(lastClickTime) < .2
		if(clickCount == 1 and shortClick) then(clickCount = 2) elseif(shortClick) then(clickCount = 1) else(clickCount = 0)

		//writeln(s, " ", clickCount)
		lastClickTime now
    )

    motion := method(mx, my,
		lastPosition copy(position)
		position set(mx, my, 0)
    )

    difference := method(
		//write("Mouse difference\n")
		//write("position:", position, "\n")
		//write("lastPosition:", lastPosition, "\n")
		tmpPoint copy(position) 
		tmpPoint -= lastPosition
		//write("diff:", tmpPoint, "\n")
		tmpPoint
    )
    
    setup := method(
		self mouseRoute := list()
		
		list("", "shift", "alt", "control") foreach(mn,
			dl := list()
			mouseRoute append(dl)
			
			list("", "double", "triple") foreach(dn,
				dn = mn .. if (mn size != 0, dn asCapitalized, dn)
				bl := list()
				dl append(bl)
				
				list("left", "middle", "right") foreach(bn,
					bn = dn .. if (dn size != 0, bn asCapitalized, bn)
					sl := list()
					bl append(sl)
					
					list("MouseDown", "MouseUp") foreach(sn,
						sn = bn .. if (bn size != 0, sn asCapitalized, sn)
						sl append(Message clone setName(sn))
					)
				)
			)
		)
    )
    
    setup
    removeSlot("setup") // so we can gc it
    
    stateMessage := method(
		mod := OpenGL glutGetModifiers
		/*
		writeln("glutGetModifiers = ", mod)
		writeln("clickCount = ", clickCount)
		writeln("button = ", button)
		writeln("state = ", state)
		*/
		//s := 
		mouseRoute at(mod) at(clickCount) at(button) at(state)
		//writeln(s)
		//s 
    )
)

