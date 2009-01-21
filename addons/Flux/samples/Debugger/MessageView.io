
Message ghost := method(
    copy := Message clone setName(self name) 
    copy setAttachedMessage(self attachedMessage ?ghost)
    copy setNextMessage(self nextMessage ?ghost)
    l := List clone
    self arguments foreach(i, v, l append(v ?ghost))
    copy setArguments(l)
    copy ghosting := self
    self ghoster := copy
    copy
)

Message lastAttachedMessage := method(
    m := self
    while (next := m attachedMessage, m = next)
    m
)

Message unOp := method(
    stackLoop(toProcess, m, self,
        if (ops hasKey(m name) and m arguments size == 1,
            arg := m argAt(0)
            arg lastAttachedMessage setAttachedMessage(m attachedMessage)
            m setArguments(list)
            m setAttachedMessage(arg)
        )
        m nextMessage and toProcess append(m nextMessage)
        m attachedMessage and toProcess append(m attachedMessage)
        m arguments size >= 0 and toProcess appendSeq(m arguments)
    )
    self
)

Map keyAtValue := method(v, keys at(values indexOf(v)))

Message unLeftOp := method(
    stackLoop(toProcess, m, self,
        if (ops leftOps hasValue(m name) and m arguments size == 2,
            arg := m argAt(1)
            arg lastAttachedMessage setAttachedMessage(m attachedMessage)
            op := Message clone setName(ops leftOps keyAtValue(m name))
            op setAttachedMessage(arg)
            m setAttachedMessage(op)
            slot := m argAt(0) name
            m setName(slot slice(1, slot size - 1))
            m setArguments(list)
        )
        m nextMessage and toProcess append(m nextMessage)
        m attachedMessage and toProcess append(m attachedMessage)
        m arguments size >= 0 and toProcess appendSeq(m arguments)
    )
    self
)

stackLoop := block(
    stackName := thisMessage argAt(0) name
    indexName := thisMessage argAt(1) name
    stack := list(sender doMessage(thisMessage argAt(2)))
    body := thisMessage argAt(3)

    sender setSlot(stackName, stack)
    while(stack size > 0,
        sender setSlot(indexName, stack pop)
        sender doMessage(body)
    )
)

ops := Map clone do (
    op := getSlot("atPut")

    op("'",   0)
    op(".",   0)
    op("?",   0)
    op("(",   0)
    op(")",   0)

    op("^",   1)

    op("++",  2)
    op("--",  2)

    op("*",   3)
    op("/",   3)
    op("%",   3)

    op("+",   4)
    op("-",   4)

    op("<<",  5)
    op(">>",  5)

    op(">",   6)
    op("<",   6)
    op("<=",  6)
    op(">=",  6)

    op("==",  7)
    op("!=",  7)

    op("&",   8)

    op("|",   9)

    op("and", 10)
    op("&&",  10)

    op("or",  11)
    op("||",  11)

    op("..",  12)

    op("=",   13)
    op("+=",  13)
    op("-=",  13)
    op("*=",  13)
    op("/=",  13)
    op("%=",  13)
    op("&=",  13)
    op("^=",  13)
    op("|=",  13)
    op("<<=", 13)
    op(">>=", 13)
    op(":=",  13)
    op("<-",  13)
    op("<->", 13)
    op("->",  13)

    op("return",  14)
    op("break",   14)
    op("super",   14)
    op("resend",  14)

    op(",", 15)
    op(";", 15)

    maxLevel := 16

    leftOps := Map clone atPut(":=", "setSlot") atPut("=", "updateSlot")

    removeSlot("op")
)

MessageView := View clone do(

  type := "MessageView"

  size set(Screen width, Screen height)
  resizeWithSuperview

  textColor ::= Point clone set(0,0,0,1)

  cursorImage := Image clone open("cursor.tif")

  msg := nil
  cursor := nil

  setMessage := method(m, msg = m ghost unOp unLeftOp; self)

  setCursor := method(m, cursor = m ?ghoster)

  crop := method(
    layoutMessage(msg, 0, 0)
    size setX(msg width, msg height)
  )

  layoutMessage := method(m, x, y,
    if (m == nil, return)
    if (m == cursor,
      m x := x + cursorImage width
    ,
      m x := x
    )
    m y := y
    m width := font widthOfString(m name)
    m height := font pixelHeight + 10
    if (m arguments size > 0, m width = m width + font widthOfString("("))
    m arguments foreach(i, v,
      layoutMessage(v, m x + m width, m y)
      if (v x != m x + m width,
        m width = m width + v x - (m x + m width)
      )
      if (i != m arguments size - 1,
        v width = v width + font widthOfString(", ")
      ,
        m width = m width + font widthOfString(")")
      )
      m width = m width + v width
    )
    if (m attachedMessage,
      m width = m width + font widthOfString(" ")
      layoutMessage(m attachedMessage, m x + m width, m y)
      if (m attachedMessage x != m x + m width,
        m width = m width + m attachedMessage x - (m x + m width)
      )
      m width = m width + m attachedMessage width
    )
    if (m nextMessage,
      layoutMessage(m nextMessage, m x, m y + m height)
      m height := m height + m nextMessage height
    )
  )
  
  drawStringAt := method(txt, x, y,
    glPushMatrix
    glTranslated(10 + x, size height - 40 - y, 0)
    font drawString(txt)
    glPopMatrix
  )

  drawCursor := method(
    if (cursor == nil or cursor hasSlot("x") == nil, return)
    glPushMatrix
    glTranslated(10 + cursor x - cursorImage width, size height - 40 - cursor y - 2, 0)
    cursorImage draw
//    font drawString(">")
    glPopMatrix 
  )

  drawMessage := method(m,
    if (m == nil, return)
    drawStringAt(m name, m x, m y)
    if (m arguments size > 0, 
      drawStringAt("(", m x + font widthOfString(m name), m y)
    )
    m arguments foreach(i, v, 
      drawMessage(v)
      if (i == m arguments size - 1,
        drawStringAt(")", v x + v width, m y)
      ,
	drawStringAt(", ", v x + v width - font widthOfString(", "), m y)
      ) 
    )
    if (m attachedMessage, drawMessage(m attachedMessage))
    if (m nextMessage, drawMessage(m nextMessage))
  )

  draw := method(
    drawBackground
    
    layoutMessage(msg, 0, 0)
   
    textColor glColor
    drawMessage(msg)

    drawCursor

  )

  messageAtPoint := method(p, m,
    if (m == nil, return nil)
    //writeln("p: ", p x, ", ", p y, " m ", m name, " ", m x, ", ", m y, " ", m width, ", ", m height)
 
    if (p y < m y - font pixelHeight, return nil)
    if (p y > m y + m height - font pixelHeight, return nil)

    m arguments foreach(i, v,
      r := messageAtPoint(p, v)
      if (r != nil, return r)
    )

    if (m attachedMessage,
      r := messageAtPoint(p, m attachedMessage)
      if (r != nil, return r)
    )

    if (m nextMessage,
      r := messageAtPoint(p, m nextMessage)
      if (r != nil, return r)
    )

    if (p x < m x, return nil)
    if (p x > m x + m width, return nil)
    if (m nextMessage and p y >= m nextMessage y - font pixelHeight, return nil)

    m
  )

  redisplay := method( glutPostRedisplay )
)
