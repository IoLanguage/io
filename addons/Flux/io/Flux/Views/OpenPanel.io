
OpenPanel := Panel clone do(
	init := method(
		resend
		self browser := Browser clone 
		browser setDelegate(self)
		browser position set(2, 55)
		browser resizeWidth = 101
		browser resizeHeight = 101
		browser resizeTo(width, height - 55 - 23)
		write("browser size = ", browser size, "\n")
		addSubview(browser)
		
		self openButton := Button clone
		openButton setTitle("Open")
		openButton position set(width - openButton width - 15, 15)
		openButton resizeWidth = 011
		openButton setActionTarget(self)
		openButton setAction("openAction")
		addSubview(openButton)
		
		self cancelButton := Button clone
		cancelButton setTitle("Cancel")
		// > Mike Austin
		// cancelButton placeLeftOf(openButton)
		// cancelButton resizeWidth = 011
		cancelButton position set( 15, 15 )
		// < Mike Austin
		cancelButton setActionTarget(self)
		cancelButton setAction("close")
		addSubview(cancelButton)
	)
	
	openAction := method(
		self openPath := browser path
		close
		actionTarget perform(action, self)
	)
	
	setPath := method(path,
		browser setRootItem(Directory clone setPath(path))
	)
	
	setRootItem := method(item,
		browser setRootItem(item)
	)
	
	itemDoubleClick := method(item,
		write("OpenPanel itemDoubleClick\n")
		//openAction
	)
)
