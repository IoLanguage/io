Yajil do(
	addValue := method(v,
		writeln("addValue ", v);
	)

	startArray := method(
		writeln("startArray");
	)

	endArray := method(
		writeln("endArray");
	)

	startMap := method(
		writeln("addMapKey ", k);
	)

	endMap := method(
		writeln("addMapKey ", k);
	)

	addMapKey := method(k,
		writeln("addMapKey ", k);
	)
)


Sequence do(
	asJson := method(SGMLParser clone elementForString(self))
)

