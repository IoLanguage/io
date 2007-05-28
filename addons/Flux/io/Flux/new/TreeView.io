// By QuantumG

TreeNode := Label clone do (

  textColor set(0, 0, 0, 1)
  lineColor := Point clone set(0.5, 0.5, 0.5, 1)

  init := method(
    self nodes := List clone
    self isExpanded := 1
    self isSelected := nil
    self expanderPos := Point clone
    resend
  )

  height := method( 
    if (font pixelHeight == 0,
      // pixelHeight returns 0 until you have actually rendered a string
      glRenderMode(GL_SELECT)
      font drawString( title )
      glRenderMode(GL_RENDER)
    )
    font pixelHeight + 6
  )

  halfTextHeight := method(font pixelHeight / 2 - 3)

  draw := method(
    lineColor glColor
    glEnable(GL_LINE_STIPPLE)
    glLineStipple(3, 0xAAAA)
    glBegin(GL_LINES)
    glVertex2d( -15, halfTextHeight )
    glVertex2d( if (nodes size == 0, -1, -8), halfTextHeight )
    glEnd
    glDisable(GL_LINE_STIPPLE)
    if (nodes size != 0,
      glBegin(GL_LINE_LOOP)
      glVertex2d( -8, halfTextHeight + 3 )
      glVertex2d( -2, halfTextHeight + 3 )
      glVertex2d( -2, halfTextHeight - 3 )
      glVertex2d( -8, halfTextHeight - 3 )
      glEnd
      glBegin(GL_POINTS)
      glVertex2d( -6, halfTextHeight )
      glVertex2d( -4, halfTextHeight )
      if (isExpanded, self, 
        glVertex2d( -5, halfTextHeight + 1 )
        glVertex2d( -5, halfTextHeight - 1 )
      )
      glVertex2d( -5, halfTextHeight )
      glEnd
    )
    glTranslated( 0, -2, 0 )
    if (isSelected, Point clone set( font widthOfString( title ) + 4, height - 5 ) drawQuad)
    glTranslated( 2, 2, 0 )
    resend
    glTranslated( -2, 0, 0 )
    start := List clone
    glGetDoublev( GL_MODELVIEW_MATRIX, start )
    expanderPos set( start at(12) - 5, start at(13) + halfTextHeight )
    glTranslated( 10, -height, 0 )
    if (isExpanded, nodes foreach(i, child, child draw ))
    glTranslated( -10, 0, 0 )
    end := List clone
    glGetDoublev( GL_MODELVIEW_MATRIX, end )
    lineColor glColor
    glEnable(GL_LINE_STIPPLE)
    glLineStipple(3, 0xAAAA)
    if (isExpanded and nodes size > 0, 
      endy := nodes at(nodes size - 1) expanderPos y
      glBegin(GL_LINES)
      glVertex2d( -5, expanderPos y - end at(13) )
      glVertex2d( -5, endy - end at(13) )
      glEnd
    )
    glDisable(GL_LINE_STIPPLE)
    self
  )

  add := method( node, nodes append( node ) )

  toggleExpanderAt := method( pos,
    tl := Point clone set( expanderPos x - 4, expanderPos y - 4 )
    br := Point clone set( expanderPos x + 4, expanderPos y + 4 )
    if (pos x > tl x and pos y > tl y and pos x < br x and pos y < br y, 
      if (isExpanded, isExpanded = nil, isExpanded = 1)
      return self 
    )
    nodes foreach(i, v, if (v toggleExpanderAt( pos ), return v))
    return nil 
  )

  selectLabelAt := method( pos,
    ty := expanderPos y - halfTextHeight - 1
    by := ty + height + 1
    if (pos y > ty and pos y < by,
      isSelected = 1
      return self
    )
    nodes foreach(i, v, n := v selectLabelAt( pos ); if (n, return n))
    return nil
  )
)

TreeView := Responder clone do (

  init := method(
    self root := nil
    self selectedLabel := nil
    resend
  )

  draw := method(
    drawBackground
    drawLineOutline
    if (root, 
      glTranslated( 10, height - root height, 0 )
      root draw
    )
  )

  add := method( label, parent,
    node := TreeNode clone 
    node setTitle( label )
    if (parent, 
      parent append( node ),
      if (root,
        root append( node ),
	root = node
      )
    )
    node
  )

  leftMouseUp := method(
    if (root == nil, return) 
    if (root toggleExpanderAt( viewMousePoint ), return)
    l := root selectLabelAt( viewMousePoint )
    if (l, 
      if (selectedLabel,
        if (selectedLabel == l, self,
	  selectedLabel isSelected = nil
	)
      )
      selectedLabel = l
      doAction
    )
  )
)
