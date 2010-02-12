ClutterVertex do(
  asList   := method(
    list(self x, self y, self z))

  asString := method(
    self asList asString exSlice(4))
)

List asClutterVertex := method(
  ClutterVertex performWithArgList("with", self))