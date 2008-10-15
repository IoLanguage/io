Sequence do(
	x := method(at(0))
	y := method(at(1))
	z := method(at(2))
	setX := method(a, atPut(0, a); self)
	setY := method(a, atPut(1, a); self)
	setZ := method(a, atPut(2, a); self)
	set := method(call evalArgs foreach(i, v, atPut(i, v)))
	rootMeanSquare := method(meanSquare sqrt)
)

Sequence addEquals := Sequence getSlot("+=")
