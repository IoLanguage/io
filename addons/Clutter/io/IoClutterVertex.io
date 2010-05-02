ClutterVertex do(
  //doc ClutterVertex asList Returns [[List]] with x, y, and z values.
  asList   := method(
    list(self x, self y, self z))

  asString := method(
    self asList asString exSlice(4))
)

//doc List asClutterVertex Converts [[List]] to [[ClutterVertex]].
List asClutterVertex := method(
  ClutterVertex performWithArgList("with", self))