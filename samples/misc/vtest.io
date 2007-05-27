#!/usr/bin/env io

Vector := Sequence clone setItemType("float32") setEncoding("number")

size := 10000
v1 := Vector clone setSize(size) rangeFill
v2 := Vector clone setSize(size) rangeFill

loops := 10000
s := Date secondsToRun(loops repeat(v1 += v2)) 

writeln(size*loops/(s*1000000000), " GFLOPS")

/*
without vectorize

1.882818 GFLOPS
1.985482 GFLOPS
1.936254 GFLOPS
1.952277 GFLOPS

with fastmath

1.941543 GFLOPS
1.918827 GFLOPS
1.896702 GFLOPS
2.135502 GFLOPS

with vectorize and fastmath

1.977812 GFLOPS
2.092639 GFLOPS
2.039853 GFLOPS
2.055593 GFLOPS
*/
