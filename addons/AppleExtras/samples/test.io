
loop(
	l := AppleSensors getLeftLightSensor
	r := AppleSensors getRightLightSensor
	b := AppleSensors getDisplayBrightness
	
	v := vector(0,0,0)
	
	AppleSensors smsVector(v)
	writeln("accelerometer = ", v)
	writeln("light sensors = (", l, ", ", r, ")")
	writeln("display brightness = ", b)
	writeln("-------------------------")
	//TimerEvent waitOn(1)
	//if (b != -1, AppleSensors setDisplayBrightness(1 - (b/1300)))
)