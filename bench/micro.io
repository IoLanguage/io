// Microbenchmark suite — emits one JSON object on stdout.
// Op-rate metrics mirror samples/speed/speed.io: millions of operations
// per second, higher is better. vectorGflops follows
// libs/iovm/samples/VectorPerformance.io.

oneMillion := 1000000

Bench := Object clone do(
	foo := method(1)

	finish := method(t1, 1 / (Date clone now secondsSince(t1)))

	localAccesses := method(
		t1 := Date clone now
		v := 1
		(oneMillion/8) repeat(v; v; v; v; v; v; v; v)
		finish(t1)
	)

	localSets := method(
		t1 := Date clone now
		v := "1"
		(oneMillion/8) repeat(v = 1; v = 2; v = 3; v = 4; v = 1; v = 2; v = 3; v = 4)
		finish(t1)
	)

	slotAccesses := method(
		self x := 1
		t1 := Date clone now
		(oneMillion/8) repeat(x; x; x; x; x; x; x; x)
		finish(t1)
	)

	slotSets := method(
		self x := 1
		updateSlot := getSlot("updateSlot")
		t1 := Date clone now
		(oneMillion/8) repeat(x = 1; x = 1; x = 1; x = 1; x = 1; x = 1; x = 1; x = 1)
		finish(t1)
	)

	blockActivations := method(
		t1 := Date clone now
		(oneMillion/8) repeat(foo; foo; foo; foo; foo; foo; foo; foo)
		finish(t1)
	)

	instantiations := method(
		t1 := Date clone now
		(oneMillion/8) repeat(
			Bench clone; Bench clone; Bench clone; Bench clone
			Bench clone; Bench clone; Bench clone; Bench clone
		)
		finish(t1)
	)

	cfuncActivations := method(
		v := self getSlot("thisContext")
		t1 := Date clone now
		(oneMillion/8) repeat(v; v; v; v; v; v; v; v)
		finish(t1)
	)

	coroutineSwitches := method(
		Lobby setSlot("benchCoroRunning", true)
		other := Object clone do(run := method(while(Lobby benchCoroRunning, yield)))
		other @@run
		yield
		n := 50000
		t1 := Date clone now
		n repeat(yield)
		dt := Date clone now secondsSince(t1)
		Lobby setSlot("benchCoroRunning", false)
		yield
		// each yield round-trips through the other coroutine: ~2 switches
		(2 * n) / dt / 1000000
	)

	hasVector := method(try(Vector; nil) == nil)

	vectorGflops := method(
		if(hasVector not, return nil)
		iters := 1000
		size := 1024
		v1 := Vector clone setSize(size) rangeFill
		v2 := Vector clone setSize(size) rangeFill
		dt := Date secondsToRun(iters repeat(v1 *= v2))
		(iters * size) / (dt * 1000000000)
	)
)

jsonNum := method(n, if(n == nil, "null", n asString(0, 3)))

pairs := List clone
pairs append(list("localAccesses", Bench localAccesses))
pairs append(list("localSets", Bench localSets))
pairs append(list("slotAccesses", Bench slotAccesses))
pairs append(list("slotSets", Bench slotSets))
pairs append(list("blockActivations", Bench blockActivations))
pairs append(list("instantiations", Bench instantiations))
pairs append(list("cfuncActivations", Bench cfuncActivations))
pairs append(list("coroutineSwitches", Bench coroutineSwitches))
pairs append(list("vectorGflops", Bench vectorGflops))

parts := pairs map(p, "\"" .. p at(0) .. "\": " .. jsonNum(p at(1)))
writeln("{" .. parts join(", ") .. ", \"ioVersion\": \"" .. System version .. "\"}")
