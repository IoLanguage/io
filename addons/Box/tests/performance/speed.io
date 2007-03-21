
m := 30000
max := 10000
a := Vector clone setSize(m+1) sin(0, m, 1, 0, 10)
b := Vector clone setSize(m+1) sin(0, m, 1, 0, 10)

writeln("// vectorSize = ", m)
writeln("// iterations = ", max)
writeln("// values are in GFLOPS\n")
writeln("Io := Object clone do(")

flops := 1000000000

t := Date cpuSecondsToRun(for(i, 0, max, a sin(0, m, 1, 0, 10)))
writeln("  sin               := ", (5*max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a mean))
writeln("  mean              := ", (max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a rootMeanSquare))
writeln("  rms               := ", (max*2*m/(t*flops)) asString(0,2)) 

write("\n")

t := Date cpuSecondsToRun(for(i, 0, max, a *= b))
writeln("  timesEquals       := ", (max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a += b))
writeln("  plusEquals        := ", (max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a -= b))
writeln("  minusEquals       := ", (max*m/(t*flops)) asString(0,2)) 

write("\n")

t := Date cpuSecondsToRun(for(i, 0, max, a *= 2))
writeln("  timesEqualsScalar := ", (max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a += 2))
writeln("  plusEqualsScalar  := ", (max*m/(t*flops)) asString(0,2)) 

t := Date cpuSecondsToRun(for(i, 0, max, a -= 2))
writeln("  minusEqualsScalar := ", (max*m/(t*flops)) asString(0,2)) 

write("\n")

t := Date cpuSecondsToRun(for(i, 0, max, a square))
writeln("  square            := ", (max*m/(t*flops)) asString(0,2))


t := Date cpuSecondsToRun(for(i, 0, max, a dot(b)))
writeln("  dot               := ", (max*m/(t*flops)) asString(0,2))

write("\n")

writeln("  version := \"", Lobby version, "\"")

writeln(")\n")

