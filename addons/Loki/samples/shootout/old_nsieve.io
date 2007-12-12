/* The Computer Language Shootout
   http://shootout.alioth.debian.org
   contributed by Robert Brandner */

Object clone do(
	
	mkbuf := method(n,
		b := List clone
		b preallocateToSize(n)
		n repeat(b append(true))
		return b
	)
	
	nsieve := method(n,
		primes := mkbuf(n)
		cnt := 0
		i := 2
		while( i<n,
			if(primes at(i),
				k := i + i
				while (k < n,
					primes atPut(k, false)
				
					k = k + i
				)
				cnt = cnt + 1
			)
			i = i + 1
		)
		writeln("Primes up to", n asString alignRight(9, " "), cnt asString alignRight(9, " "))
	)
	
	clk := chrono(
	//n := args at(1) asNumber
	nsieve( (2**(n))*10000 )
	nsieve( (2**(n-1))*10000 )
	nsieve( (2**(n-2))*10000 )
	)

	"#{clk / (2* (10 **(9)))} sec" interpolate println
)
"ok."