	
ColumnView := VScrollArea clone do(
	textures := TextureGroup clone loadGroupNamed("ColumnView")
	selectedItem := nil
	delegate ::= nil
		
	init := method(
		resend
		self stackView := VStackView clone
		stackView resizeWidth = 101
		clipView setDocumentView(stackView)
		stackView resizeWidthTo(clipView width)
		addRow
	)
	
	rows := method(stackView subviews)
	
	empty := method(rows empty)
	
	addRow := method(
		stackView addSubview(TextCell clone setTitle("test"))
	)
	
	setRowObjects := method(objects,
		empty
		objects reverseForeach(obj, 
			if(obj == nil, write("obj == nil\n"); exit)
			t := TextCell clone setObject(obj)
			t resizeWidthTo(clipView width)
			t setActionTarget(self)
			t setAction("selectedItemAction")
			stackView addSubview(t)
		)
	)
	
	selectedItem := method(
		rows detect(isSelected)
	)
	
	selectedIndex := method(
        index := rows indexOf(selectedItem)
        if(index == nil, 0, index)
    )
    
	selectIndex := method(index,
	   if(index < 0 or index > rows size - 1, return)
	   selectedItem ?unselect
	   selectedItemAction(rows at(index))
	   glutPostRedisplay
	)
	
	selectNextItem := method(
	   selectIndex(selectedIndex - 1)
	)
	
	selectPreviousItem := method(
	   selectIndex(selectedIndex + 1)
	)
	
	selectedItemAction := method(anItem,
		rows foreach(unselect)
		anItem select
		delegate ?columnSelected(self, anItem)
	)
	
	itemDoubleClick := method(item,
		write("ColumnView itemDoubleClick\n")
		delegate ?itemDoubleClick(item)
	)
	
	draw := method(
		resend 
		textures draw(width, height)
	)
	
	special := method(key,
	   if(key == GLUT_KEY_UP) then(selectPreviousItem
           ) elseif(key == GLUT_KEY_DOWN) then(selectNextItem
           ) elseif(key == GLUT_KEY_LEFT) then(delegate ?selectColumnBefore(self)
           ) elseif(key == GLUT_KEY_RIGHT) then(delegate ?selectColumnAfter(self)
           ) else(resend)
	)
)
