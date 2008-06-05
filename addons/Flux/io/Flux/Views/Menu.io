
Menu := View clone do(
	items := List clone
	itemSize := Point clone set(100, 26)
	selectedItem := nil
	
	init := method(
		resend
		items = items clone
		itemSize = itemSize clone
		outlineColor set(.5,.5,.5, 1)
		clippingOff
	)
	
	addItem := method(item,
		items append(item)
		self addSubview(item)
		setup
	)
	
	removeItem := method(item,
		items remove(item)
		self removeSubview(item)
		if (selectedItem == item, selectedItem = nil)
		setup
	)
	
	addItemNamed := method(name,
		item := MenuItem clone setTitle(name)
		self addItem(item)
		if (selectedItem == nil, selectedItem = item)
	)
	
	removeItemNamed := method(name,
		item foreach(item, 
			if(item title == name, self removeItem(item); return)
		)
	)
	
	setup := method(
		size set(itemSize width, items size * itemSize height)
		y := 0
		items reverseForeach(item,
			item position set(0, y)
			y = y + itemSize height
			item size copy(itemSize)
		)
	)
	
	draw := method(
		setup
		self drawBackground
		self drawOutline
	)
	
	mouse := method(
		if(Mouse state == 1,
			item := self tmpSelection
			if(item,
				selectedItem = item
				items foreach(item, item unselect)
				selectedItem select
			)
		)
	)
	
	alignWithSelection := method(
		if (selectedItem,
			index := items indexOf(selectedItem)
			offset := ((items size - index - 1) * itemSize height)
			position y = position y - offset
		)
	)
	
	motion := method(
		p := viewMousePoint
		index := (size height - p y) / itemSize height
		index Floor
		items foreach(item, item tmpUnselect)
		if(p x < 0 or p x > size width, return)
		if(index < items size and index > -1, items at(index) tmpSelect)
		//write("Menu motion ", Mouse position y, "\n")
		if(Mouse position y < 0 and viewToScreen(Point clone) y < 0,
			position setY(position y + itemSize height/2)
		)
		
		//write("viewToScreen(Point clone) y ", viewToScreen(Point clone) y, "\n")
		//write("viewToScreen(Point clone) y + size height ", viewToScreen(Point clone) y + size height, "\n")
		if(Mouse position y > Screen height and viewToScreen(Point clone) y + size height > Screen height,
			position setY(position y - itemSize height/2)
		)
	)
	
	tmpSelection := method(
		items foreach(item, if(item isTmpSelected == 1, return item))
		nil
	)
)
