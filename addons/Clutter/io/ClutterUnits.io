ClutterUnits do(
)

Sequence asClutterUnit := method(
  ClutterUnit withString(self))

Number do(
  px := method(ClutterUnits withPx(self))
  mm := method(ClutterUnits withMm(self))
  pt := method(ClutterUnits withPt(self))
  em := method(ClutterUnits withEm(self))
  emForFont := method(font,
    ClutterUnits withEmForFont(font, self))
)