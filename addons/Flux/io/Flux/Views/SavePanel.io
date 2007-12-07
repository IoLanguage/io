
Object show := method(
	args := call message arguments
	args foreach(m,
		write(m name, " = ", call sender doMessage(m))
		if (m != args last, write(", "))
	)
	write("\n")
)

SavePanel := OpenPanel clone do(
	init := method(
		resend
		self textField := TextField clone 
		textField resizeWidth  = 101
		textField resizeHeight = 011
		browser resizeTo(browser width, browser height - 40)	
		y := browser position y + browser height + 10
		show(y)
		textField position set(90, height - 55)
		textField setWidth(width - 100)
		textField setString("")
		addSubview(textField)
		
		self label := Label clone
		label resizeWidth = 110
		label resizeHeight = 011
		label setTitle("Save As:")
		label setWidthToTitle
		label placeLeftOf(textField)
		label position setY(label position y + 5)
		addSubview(label)
		
		openButton setTitle("Save")
	)
	
	setFileName := method(name, textField setString(name))
	
	savePath := method(
		fileName := textField string
		if (openPath endsWithSeq(fileName), return openPath)
		Path with(openPath, fileName)
	)
	
	browserSelectedItem := method(item,
		if (item object hasSubitems not, textField setString(item object name))
	)
)
