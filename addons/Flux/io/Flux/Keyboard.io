
Keyboard := Object clone do(
    init := method(
		self keys := List clone
		for(i, 0, 255, keys append(0))
		
		self specialKeys := List clone
		for(i, 0, 255, specialKeys append(0))
    )
    
    init

    // -- input ---------------------

    key := method(key, x, y, state,
		//write("Keyboard key ", key, "\n")
		keys atPut(key, state)
    )

    special := method(key, x, y, state,
		//write("Keyboard special ", key, "\n")
		specialKeys atPut(key, state)
    )
    
    shiftKeyIsDown := method((GLUT glutGetModifiers & GLUT GLUT_ACTIVE_SHIFT) != 0)
    controlKeyIsDown := method((GLUT glutGetModifiers & GLUT GLUT_ACTIVE_CTRL) != 0)
    altKeyIsDown := method((GLUT glutGetModifiers & GLUT GLUT_ACTIVE_ALT) != 0)
	
    // -- getters ---------------------

    stateOfKey := method(k, keys at(k))
    stateOfSpecialKey := method(k, specialKeys at(k))
    //controlKeyDown := method(stateOfSpecialKey(1024))
    
    isShiftKeyDown := method((GLUT glutGetModifiers & GLUT GLUT_ACTIVE_SHIFT) != 0)
)
