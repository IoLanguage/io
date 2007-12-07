writeln("Executing config file")

// In this script you can manipulate the objects created by china.io directly,
// but preferrably you should pass config values via the chinaConfigMap.

// Put values (after the comma) to be interpreted by china.io into this Map:

chinaConfigMap := Map clone do(
	//atPut("user", "my friend")	// put your name here
	//atPut("bkgndColor", Color clone set(0.6, 0.8, 0.6, 1))	// r, g, b, alpha
	
	// The horInc (horizontal increment) value determines the width and height
	// of the board graphics. On some systems the default value of 32 may not
	// give nice looking graphics, and 30 or 40 may be better.
	atPut("horInc", 40)
	
	// The winWidth and winHeight give the dimension of the window.
	// These values should be roughly 20 times the horInc value.
	atPut("winWidth", 800)
	atPut("winHeight", 760)
	
	//atPut("useAllColors", 1)	// if you want to play 3 colors against 3
	atPut("shortJumps", 1)	// Nil gives LONG jumps
)

// Return the Map
chinaConfigMap
