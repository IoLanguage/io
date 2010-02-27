ClutterEvent do(
  init := method(
    self x = 0
    self y = 0)

  0_coords_ := getSlot("coords")
  coords = lazySlot(
    realCoords := self 0_coords_
    self setX(realCoord x) setY(realCoords y)
    realCoords)

  x ::= method(
    self coords x)
  y ::= method(
    self coords y)
)
