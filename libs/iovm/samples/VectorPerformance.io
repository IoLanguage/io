

iters := 1000
size := 1024
ops := iters * size

v1 := Vector clone setSize(size) rangeFill
v2 := Vector clone setSize(size) rangeFill

dt := Date secondsToRun(
	iters repeat(v1 *= v2)
)

writeln(dt, " seconds for ", ops, " ops")
writeln((ops/(dt*1000000000)) asString(1, 3), " GFLOPS")


