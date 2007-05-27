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
    if (n > 0 and n <= 10) then (
        H(n, 1, 2, 3)
    ) else (
        writeln("usage: hanoi n, where 0 < n <= 10")
    )
)

hanoi(3)
