ClutterEvent do(
  init := method(
    self x = 0
    self y = 0)

  __coords := getSlot("coords")
  coords = lazySlot(
    realCoords := self __coords
    self setX(realCoord x) setY(realCoords y)
    realCoords)

  //doc ClutterEvent x
  x ::= method(
    self coords x)
  //doc ClutterEvent y
  y ::= method(
    self coords y)
)
