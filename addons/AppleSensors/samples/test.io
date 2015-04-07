#!/usr/bin/env io

sensors := AppleSensors clone

loop(
	LS := sensors getLeftLightSensor
	RS := sensors getRightLightSensor
	DB := sensors getDisplayBrightness
	CPU := sensors getCPUTemperature
	GPU := sensors getGPUTemperature
	PLM := sensors getPalmTemperature
	BAT := sensors getBatteryTemperature
	RAM := sensors getRAMTemperature
	PCH := sensors getPCHTemperature
	PWR := sensors getPowerTemperature
	
	//v := vector(0,0,0)
	
	//sensors smsVector(v)
	//writeln("accelerometer = ", v)
	writeln("light sensors = (", LS, ", ", RS, ")")
	writeln("display brightness = ", DB)
	writeln("Temperatures = ({CPU:", CPU, "},{GPU:", GPU, "},{Palm:", PLM, "},{Battery:", BAT, "},{RAM:", RAM, "},{PlatformControllerHub:", PCH, "},{Power:", PWR, "})")
	writeln("----------------------------------------------------")
	//TimerEvent waitOn(1)
	//if (b != -1, AppleSensors setDisplayBrightness(1 - (b/1300)))
)

sensors free