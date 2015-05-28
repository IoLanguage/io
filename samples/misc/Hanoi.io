#!/usr/bin/env io

H := method(n, f, u, t,
    if (n < 2) then (
        writeln(f, " --> ", t)
    ) else (
        H(n - 1, f, t, u)
        writeln(f, " --> ", t)
        H(n - 1, u, f, t)
    )
)

hanoi := method(n,
	i := n
    if (n > 0 and n <= i) then (
        H(n, 1, 2, 3)
    ) else (
        writeln("usage: hanoi n, where n > 0")
    )
)

hanoi(12)