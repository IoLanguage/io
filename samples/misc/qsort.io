List do(
	qsort := method(
		if(size < 2, return)
		quickSort(0, size - 1)
	)

	quickSort := method(b, t,
		loop(
			if (b >= t, return)
			
			lb := b
			ub := t
	
			loop(		
				while (ub >(lb) and(at(ub) compare(at(lb)) >= 0), 
					ub = ub - 1
				)
				
				if (ub != lb,
					swapIndices(ub, lb)
					while (lb <(ub) and(at(lb) compare(at(ub)) <= 0), lb = lb + 1)
					if (lb != ub, swapIndices(lb, ub))
				)
				
				if(lb == ub, break)
			)
			
			if (lb != 0,
				//write("-")
				quickSort(b, lb - 1)
			)
			b = lb + 1
		)
	)

	quickSortRearrange := method(lb, ub,
		loop(		
			while ((ub > lb) and(at(ub) >= at(lb)), 
				ub = ub - 1
			)
			
			if (ub != lb,
				swapIndices(ub, lb)
				while ((lb < ub) and(at(ub) >= at(lb)), lb = lb + 1)
				if (lb != ub, swapIndices(lb, ub))
			)
			
			if(lb == ub, break)
		)
		lb
	)
)

writeln("building list")
a := list()
10000 repeat(i, a append(i asString))
a shuffle

testSort := method(
	a clone doMessage(call argAt(0))
)

writeln("Testing")

for(i, 0, 3,
	writeln("testQSort ", Date secondsToRun(testSort(qsort)))
	writeln("testSort  ", Date secondsToRun(testSort(sortInPlace)))
	"" println
)
