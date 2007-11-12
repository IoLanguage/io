
OSMenuEntry := GLObject clone do(
	id := method(self uniqueId)
	newSlot("name")
	
	open := method(
		glutAddMenuEntry(name, id)
		self
	)
	
	addItem := method(name,
	glutAddMenuEntry(name, menuEntry id)
	)
)

OSMenu := GLObject clone do(
	newSlot("id")
	newSlot("entries")
	
	init := method(
		resend
		setEntries(List clone)
	)
	
	open := method(
		setId(glutCreateMenu)
		self
	)
	
	addEntryNamed := method(name,
		m := OSMenuEntry clone setName(name)
		glutSetMenu(id)
		entries append(m)
		m open
		m
	)	
	
	test := method(
		mid := glutCreateMenu
    	writeln("mid = ", mid)
  		glutSetMenu(mid)
  		glutAddMenuEntry("foo1", 1)
  		glutAddMenuEntry("foo2", 2)
  		glutAddMenuEntry("foo3", 3)
	)
)

