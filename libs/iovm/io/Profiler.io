Block printProfile := method(
	m := getSlot("self") message
	writeln("  ", getSlot("self") profilerTime asString(0,6), "s in Block (", getSlot("self") argumentNames join(","), ") ", m label, " ", m lineNumber)
)

Core getSlot("CFunction") printProfile := method(
	writeln("  ", getSlot("self") profilerTime asString(0,6), "s in CFunction ", getSlot("self") uniqueName, "()")
	
)

Profiler do(
	start := method(
		self reset
		Block setProfilerOn(true)
		//CFunction setProfilerOn(true)
		Core getSlot("CFunction") setProfilerOn(true)
		self
	)
	
	stop := method(
		Block setProfilerOn(false)
		//CFunction setProfilerOn(false)
		Core getSlot("CFunction") setProfilerOn(false)
		self
	)
	
	show := method(
		writeln("\nProfile:")
		blocks := self timedObjects select(b, getSlot("b") type == "Block") // or getSlot("b") type == "CFunction")
		blocks := blocks map(b, 
			o := Object clone
			o newSlot("b", getSlot("b"))
			o newSlot("profilerTime", getSlot("b") profilerTime) 
			o
		) 
		blocks := blocks sortByKey(profilerTime) reverse select(profilerTime > 0)
		if(blocks size == 0, 
			writeln("  sample size to small")
		,
			maxTime := blocks first profilerTime
			blocks foreach(b,
				b getSlot("b") printProfile
				if(b profilerTime < maxTime/100, break)
			)
		)
		writeln("")
	)
	
	profile := method(
		start
		call evalArgAt(0)
		stop
		show
	)
	
)
