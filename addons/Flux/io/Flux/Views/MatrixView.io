
MatrixView := View clone do(
    cell ::= nil
	setCell := method(aCell,
		cell = aCell
		cell setSuperview(self)
		self
	)
    scroller ::= nil
    delegate ::= nil

	init := method(
		resend
		self selectedIndexes := List clone
		self minSelection := Point clone
		self maxSelection := Point clone
	)

	numberOfColumns := method(
		(width / cell width) floor
	)

	numberOfRows := method(
		(numberOfItems / numberOfColumns max(1))  ceil
    )

	numberOfItems := method(
		delegate numberOfItems(self)
	)
	
	didChangeSize := method(
		//writeln("matrixView didChangeSize")
		resizeToFitItems
	)
	
	//update := method(resizeToFitItems)
	
	resizeToFitItems := method(
		//writeln("resizeToFitItems")
		setHeight((numberOfRows * cell height) floor)
		setWidth(superview width floor)
		//superview update
	)
	
    draw := method(		
		//glRecti(0, 0, width, height)
		offset := superview ?positionOffset
		
    	numberOfItems   := numberOfItems
		numberOfColumns := numberOfColumns 
		numberOfRows    := numberOfRows

		if(offset,
			startHeight := ( (- offset y) floor)
        	minRow := ((startHeight / cell height) floor)
        	maxRow := minRow + (superview height / cell height) ceil
			//writeln("startHeight = ", startHeight) 
			//writeln("superview height = ", superview height)
			//writeln(minRow, " - ", maxRow, " of ", numberOfRows)
		,
			minRow := 0
			maxRow := numberOfRows
		)
		
		//glTranslatei(0, height - cell height, 0)
		
        for(row, minRow, maxRow,
			for(column, 0, numberOfColumns - 1,
	            glPushMatrix
	        	glTranslatei(column * cell width, row * cell height, 0)
				index :=  (numberOfRows - row) * numberOfColumns + column
				//writeln(row, ".", column)
				cell setHasSelection(selectedIndexes isEmpty not)
				cell setSelected(selectedIndexes contains(index))
           		delegate willDrawCellNumber(cell, index)
	            cell display
		        glPopMatrix     
			)
        )
    )

	rowColumnAtMousePosition := method(
		p := viewMousePoint
		p setX((p x / cell width) floor)
		p setY(numberOfRows - ((p y) / cell height) floor)
		p
	)
	
	indexForColumnRow := method(c, r,
	 	r * numberOfColumns + c
	)

	leftMouseDown := method(mouse,
		cr := rowColumnAtMousePosition
		self startCr := Vector clone copy(cr)
		index := indexForColumnRow(cr x, cr y)
		if (index == selectedIndexes first and selectedIndexes size == 1, 
			selectedIndexes removeAll
			return
		)
		minSelection copy(cr)
		maxSelection copy(cr)
		applySelectionRange
		makeFirstResponder
	)
	
	doubleLeftMouseDown := method(
		cr := rowColumnAtMousePosition
		index := indexForColumnRow(cr x, cr y)
		delegate willDrawCellNumber(cell, index)
   		cell doubleLeftMouseDown
	)
	
	applySelectionRange := method(
		selectedIndexes removeAll
		for(x, minSelection x, maxSelection x,
			for(y, minSelection y, maxSelection y,
				selectedIndexes append(indexForColumnRow(x, y))				
			)
		)
		self
	)

	selectedIndex := method(
		selectedIndexes first ifNil(0)
	)
	
	setSelectedIndex := method(i,
		if(i < 0 or i > numberOfItems - 1, return)
		selectedIndexes removeAll append(i)
	)
	
	keyboardRightArrow	:= method(setSelectedIndex(selectedIndex+1))
	keyboardLeftArrow 	:= method(setSelectedIndex(selectedIndex-1))
	keyboardDownArrow 	:= method(setSelectedIndex(selectedIndex+numberOfColumns))
	keyboardUpArrow 	:= method(setSelectedIndex(selectedIndex-numberOfColumns))
	
	keyboardDelete := method(
		i := selectedIndex
		selectedIndexes foreach(index, MediaStore removeAtIndex(index))
		MediaStore findTags(list("image"))
		setSelectedIndex(i)
		setNeedsRedraw(true)
	)
	
	leftMouseMotion := method(
		cr := rowColumnAtMousePosition
		minSelection copy(startCr) Min(cr)
		maxSelection copy(startCr) Max(cr)
		applySelectionRange
		setNeedsRedraw(true)
		self
	)
)