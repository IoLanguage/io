// by Trent Waddington

Tag := View clone do(
	init := method(
		self line := nil
		font := fonts system normal
	)
	
	height := method(
		if (font pixelHeight == 0,
			glRenderMode(GL_SELECT)
			font drawString( "()" )
			glRenderMode(GL_RENDER)
		)
		font pixelHeight
	)
	
	boundX := method(x, 0)
	atXInsert := method(x, str, nil)
	atXDelete := method(x, nil)
	atXBreak := method(x, nil)
	
	width := 0
)

TabTag := Tag clone do(
	draw := method(
		glTranslated(font widthOfString("  "), 0, 0)
	)
	
	width := method(font widthOfString("  "))
	
	asString := "\t"
)

TextTag := Tag clone do(
	init := method(
		resend
		self text := ""
	)

	setText := method(t, text = t; self)
	
	draw := method(
		font drawString(text)
		glTranslated(font widthOfString(text), 0, 0)
	)
	
	width := method(font widthOfString(text))
	
	boundX := method(x,
		for(n, 0, text size,
			p := font widthOfString(text substring(0, n))
			w := font widthOfString(text substring(n, n + 1))
			if(x >= p and x < p + w, return p)
		)
		width
	)
	
	atXInsert := method(x, str, 
		for (n, 0, text size,
			p := font widthOfString(text slice(0, n))
			if (x == p,
				text = text atInsertSeq(n, str)
				l := font widthOfString(text slice(0, n + str size))
				return l
			)
		)
		text appendSeq(str)
		width
	)
	
	atXDelete := method(x,
		for (n, 0, text size,
			p := font widthOfString(text substring(0, n))
			if (x == p,
				text = text removeSlice(n, n)
				return 1
			)
		)
		nil
	)
	
	atXBreak := method(x,
		for (n, 0, text size,
			p := font widthOfString(text substring(0, n))
			if (x == p,
				t := TextTag clone setText(text substring(n))
				text = text substring(0, n)
				return t
			)
		)
		nil
	)
	
	asString := method(text)
)

SpaceTag := Tag clone do(
	draw := method(glTranslated(font widthOfString(" "), 0, 0))
	width := method(font widthOfString(" "))
	asString := " "
)

Line := View clone do(
	
	init := method(
		self tags := List clone
	)
	
	add := method(v, tags append(v))
	remove := method(v, tags remove(v))
	
	height := method(
		if (tags size == 0, return font pixelHeight)
		maxHeight := 0
		tags foreach(i, v, if (v height > maxHeight, maxHeight = v height))
		maxHeight
	)
	
	width := method(
		w := 0
		tags foreach(i, v, w = w + v width)
		w
	)
	
	draw := method(
		glPushMatrix
		tags foreach(i, v, v draw)
		glPopMatrix
		glTranslated(0, height, 0)
	)
	
	boundX := method(x,
		w := 0
		tags foreach(i, v, 
			if(x >= w and x < w + v width,
				return w + v boundX(x - w)
			)
			w = w + v width
		)
		w
	)
	
	atXInsert := method(x, str,
		w := 0
		n := tags size
		tags foreach(i, v,
			if(x >= w and x < w + v width,
				if(str == " " or str == "\t",
					nt := v atXBreak(x - w)
					w = w + v width
					tags atInsert(i + 1, nt)
					n = i + 1,
					nx := v atXInsert(x - w, str)
					if(nx, return w + nx)
					n = i
				)
				break
			)
			w = w + v width
		)
		if(str == " ",
			t := SpaceTag clone,
			if(str == "\t",
				t := TabTag clone,
				t := TextTag clone setText(str)
			)
		)
		tags atInsert(n, t)
		w + t width
	)
	
	atXDelete := method(x,
		w := 0
		n := tags size
		tags foreach(i, v,
			if(x >= w and x < w + v width,
				nx := v atXDelete(x - w)
				if(nx, return)
				tags removeAt(i)
				return
			)
			w = w + v width
		)
	)
	
	atXBreak := method(x,
		nl := Line clone
		w := 0
		e := nil
		tags foreach(i, v,
			if (nl tags size > 0,
				nl append(v), 
				if (x >= w and x < w + v width,
					n := v atXBreak(x - w)
					if (n,
						nl append(n)
						e = i + 1,
						nl append(v)
						e = i
					)
				)
			)
			w = w + v width
		)
		if (e, 
		for (i, 1, tags size - e,
		tags removeAt(e)
		)
		)
		nl
	)
	
	moveLeft := method(x,
		n := 1
		while(x - n > 0,
			nx := boundX(x - n)
			if (nx == x, n = n + 1, return nx)
		)
		x
	)
	
	moveRight := method(x,
		n := 1
		while(x + n <= width,
			nx := boundX(x + n)
			if (nx == x, n = n + 1, return nx)
		)
		x
	)
	
	asString := method(
		s := Sequence clone
		tags foreach(i, v, s appendSeq(v asString))
		s appendSeq("\n") asSymbol
	)
)

LinesList := List clone do(
	totalHeight := method(
		h := 0
		self foreach(i, v, h = h + v height)
		h
	)
	
	getVisible := method(start, height,
		visible := LinesList clone
		h := 0
		self foreach(i, v, 
			if (i < start, continue)
			if (h + v height > height, break) 
			visible append(v)
			h = h + v height
		)
		visible
	)
	
	lineForY := method(y,
		h := 0
		self reverseForeach(i, v,
			h = h + v height
			if(y < h, return v)
		)
		self first
	)
)

Editor := View clone do(
	init := method(
		self lines := LinesList clone
		self cursorLine := nil
		self cursorX := 0
		self cursorFlashing := nil
		self firstDraw := 1
		self startLine := 0
		self lastVisibleLine := nil
	)
	
	load := method(path,
		f := File clone openForReading(path)
		if (f == nil, return)
		flines := f readLines
		f close
		
		lines empty
		
		flines foreach(lineno, line,
			l := Line clone
			tt := TextTag clone
			for(i, 0, line size - 1,
				if (i >= line size, break)
				if (line at(i) asCharacter == " ") then(
					
					if (tt text size > 0,
						l append(tt)
						tt = TextTag clone
					)
					
					l append(SpaceTag clone)
					
				) elseif (line at(i) asCharacter == "\t",
					if (tt text size > 0,
						l append(tt)
						tt = TextTag clone
					)
					l append(TabTag clone),
					tt text = tt text appendSeq(line at(i) asCharacter)
				)
			)
			if (tt text size > 0, l append(tt))
			lines append(l)
		)
		cursorLine = lines first
	)
	
	save := method(path,
		f := File clone openForUpdating(path)
		if (f == nil, return)
		lines foreach(i, v, f write(v asString))
		f close
	)
	
	draw := method(
		if (firstDraw,
			firstDraw = nil
			Screen addTimerTargetWithDelay(self, 0.5)
		)
		
		visibleLines := lines getVisible(startLine, height)
		lastVisibleLine = visibleLines last
		glTranslated(0, height - visibleLines totalHeight, 0)
		glPushMatrix
		visibleLines reverseForeach(i, v, 
			if (cursorFlashing == nil,
				if (cursorLine == v,
					glBegin(GL_LINES)
					glVertex2i(cursorX, -5)
					glVertex2i(cursorX, v height)
					glEnd
				)
			)
			v draw
		)
		glPopMatrix
	)
	
	timer := method( 
		if (cursorFlashing,
		cursorFlashing = nil,
		cursorFlashing = 1)
		glutPostRedisplay 
		Screen addTimerTargetWithDelay(self, 0.5)
	)
	
	setSize := method(x, y, size set(x, y); self)
	
	leftMouseDown := method(
		h := viewMousePoint y - height - lines totalHeight
		cursorLine = lines lineForY(h) 
		cursorX = cursorLine boundX(viewMousePoint x)
	)
	
	special := method(key, x, y,
		if (key == GLUT_KEY_LEFT,
			if (cursorX == 0,
				if (cursorLine == lines first, return)
				cursorLine = lines at(lines indexOf(cursorLine) - 1)
				cursorX = cursorLine width,
				cursorX = cursorLine moveLeft(cursorX)
			)
			return
		)
		
		if (key == GLUT_KEY_UP,
			if (cursorLine == lines first, return)
			if (lines indexOf(cursorLine) <= startLine, 
				startLine = startLine - 1
			)
			cursorLine = lines at(lines indexOf(cursorLine) - 1)
			cursorX = cursorLine boundX(cursorX)
			return
		)
		
		if (key == GLUT_KEY_RIGHT,
			if (cursorX == cursorLine width,
			if (cursorLine == lines last, return)
				cursorLine = lines at(lines indexOf(cursorLine) + 1)
				cursorX = 0,
				cursorX = cursorLine moveRight(cursorX)
			)
			return
		)
		
		if (key == GLUT_KEY_DOWN,
			if (cursorLine == lines last, return)
			if (lines indexOf(cursorLine) >= lines indexOf(lastVisibleLine), 
				startLine = startLine + 1
			)
			cursorLine = lines at(lines indexOf(cursorLine) + 1)
			cursorX = cursorLine boundX(cursorX)
			return
		)
		
		if (key == GLUT_KEY_PAGE_UP,
			if (startLine < 10,
				startLine = 0,
				startLine = startLine - 10
			)
			cursorLine = lines at(startLine)
			cursorX = cursorLine boundX(cursorX)
			return
		)
		
		if (key == GLUT_KEY_PAGE_DOWN,
			if (startLine > lines size - 20,
				startLine = lines size - 10,
				startLine = startLine + 10
			)
			cursorLine = lines at(startLine)
			cursorX = cursorLine boundX(cursorX)
			return
		)
		//write("special: ", key, "\n")
	)
	
	keyboard := method(key, x, y,
		if (key == GLUT_KEY_DELETE,
		if (cursorX == 0, 
		if (cursorLine == lines first, return)
			prevLine := lines at(lines indexOf(cursorLine) - 1)
			cursorX = prevLine width
			prevLine tags addSeq(cursorLine tags)
			lines remove(cursorLine)
			cursorLine = prevLine
			return
		)
		cursorX = cursorLine moveLeft(cursorX)
			cursorLine atXDelete(cursorX)
			return
		)
		
		if (key == GLUT_KEY_DEL,
			cursorLine atXDelete(cursorX)
			return
		)
		
		if (key == GLUT_KEY_RETURN,
			l := cursorLine atXBreak(cursorX)
			lines atInsert(lines indexOf(cursorLine) + 1, l)
			cursorLine = l
			cursorX = 0
			return
		)
		
		t := cursorLine atXInsert(cursorX, key asCharacter)
		cursorX = t
		//write("key: ", key, "\n")
	)
)
