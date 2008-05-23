
// The Browser item protocol is:
// title, subitems, hasSubitems 

Directory do(
    title := method(name)
    subitems := method(
      items select(name beginsWithSeq(".") not)
    )
    object := nil
    hasSubitems := 1
)

File do(
    title := method(name)
    subitems := method(List clone)
    hasSubitems := nil
)

BrowserDelegate := Directory clone setPath("/")

Browser := HScrollArea clone do(
    columnWidth ::= 210
    rootItem := BrowserDelegate clone
    textures := TextureGroup clone loadGroupNamed("Browser")
    delegate ::= nil
    
    setRootItem := method(item, 
		rootItem = item
		updateColumns   
    )
    
    init := method(
		resend
		self stackView := HStackView clone
		stackView resizeWidth  = 110
		stackView resizeHeight = 101
		stackView setHeight(clipView height)
		clipView setDocumentView(stackView)
		addColumn
		updateColumns
    )
    
    addColumn := method(
		c := ColumnView clone
		c forceResizeTo(columnWidth, c height)
		c setDelegate(self)
		stackView addSubview(c)
		clipView update
		c
    )
    
    columns := method(stackView subviews)

    emptyColumns := method(
		columns foreach(column, 
			column empty
			column stackView organize
		)
    )
        
    updateColumns := method(
		emptyColumns
		columns first setRowObjects(rootItem subitems)
    )
    
    selectedItem := nil
    
    columnSelected := method(column, item,
        selectedItem = item
        i := columns indexOf(column)
        while (columns size > i + 1, columns last removeFromSuperview)

        if (column == columns last and item object hasSubitems,
            addColumn setRowObjects(item object subitems)
        )
        
        if (delegate, delegate ?browserSelectedItem(item))
        scroller setValue(1) doAction
        glutPostRedisplay
    )
    
    itemDoubleClick := method(item,
		write("Browser itemDoubleClick\n")
		if (delegate, delegate ?itemDoubleClick(item))
    )
    
    path := method(
		if (selectedItem, return selectedItem object path)
		rootItem path
    )
    
    draw := method(
    	if(isTextured, textures draw(width, height), drawRoundedBox)
    )
    
    selectColumnBefore := method(column,
        c := columns itemBefore(column)
        if(c, 
            column selectedItem ?unselect
            columnSelected(c, c selectedItem)
            c makeFirstResponder
        )
    )
    
    selectColumnAfter := method(column,
        c := columns itemAfter(column)
        if(c, 
            c selectIndex(c rows size - 1) 
            columnSelected(c, c rows last)
            c makeFirstResponder
        )
    )
)

List itemAfter  := method(v, self at(self indexOf(v) + 1))
List itemBefore := method(v, self at(self indexOf(v) - 1))
