
ObjectView = View clone
ObjectView rowHeight = 30
ObjectView target = Object clone
ObjectView init = block(  
  write("ObjectView init ", self numberId, "\n")
  resend
  self setTarget(Object clone)
)

ObjectView setTarget = block(target,
  self target = target

  self subviews empty
  names = target slotNames sort reverse
  self size setHeight(names size * rowHeight + rowHeight);
  self size setWidth(300);
  
  y = 0
  names foreach(i, name,
    slotView = SlotView clone
    slotView name = name
    slotView position setY(y)
    slotView size setHeight(rowHeight) setWidth(self size width)
    self addSubview(slotView)
    y = y + rowHeight
  )  
)

ObjectView draw = block(
  super(drawOutline)
)

ObjectView mouse = block(
  self
)



