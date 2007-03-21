
SlotView = View clone
SlotView isSelected = 0
SlotView delegate = nil
SlotView name = "?"
SlotView size setHeight(20) setWidth(300)
SlotView init = block(
  resend
  button = ButtonView clone
  button size setX(15) setY(15)
  button position setX(self size width - button size width - 7) setY(7)
  self addSubview(button)
)

SlotView draw = block(
  super(drawOutline)
  if(isSelected == 1, glColor4d(0,1,0,1), outlineColor glColor)
  Font draw(name asUppercase, self size height - 2, 5, 5)
  outlineColor glColor
)

SlotView buttonClick = block(
  "SlotView buttonClick\n" print
  glutPostRedisplay
)

SlotView motion = block(self)




