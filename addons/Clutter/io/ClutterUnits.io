ClutterUnits do(
)

//doc Sequence asClutterUnit Calls [[ClutterUnit withString]].
Sequence asClutterUnit := method(
  ClutterUnit withString(self))

Number do(
  //doc Number px Calls [[ClutterUnits withPx]].
  px := method(ClutterUnits withPx(self))
  //doc Number mm Calls [[ClutterUnits withMm]].
  mm := method(ClutterUnits withMm(self))
  //doc Number pt Calls [[ClutterUnits withPt]].
  pt := method(ClutterUnits withPt(self))
  //doc Number em Calls [[ClutterUnits withEm]].
  em := method(ClutterUnits withEm(self))
  //doc Number emForFont(fontname) Calls [[ClutterUnits emForFont]].
  emForFont := method(font,
    ClutterUnits withEmForFont(font, self))
)