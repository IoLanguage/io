
enum {
	kGetSensorReadingID = 0, // getSensorReading(int *, int *)
	kGetLEDBrightnessID = 1, // getLEDBrightness(int, int *)
	kSetLEDBrightnessID = 2, // setLEDBrightness(int, int, int *)
	kSetLEDFadeID = 3, // setLEDFade(int, int, int, int *)
	// other firmware-related functions
	// verifyFirmwareID = 4,
	// verifyFirmware(int *)
	// getFirmwareVersionID = 5,
	// getFirmwareVersion(int *)
	// other flashing-related functions
	// ...
};


#include "AppleLMU.h"

static io_connect_t lmuDataPort = 0; // shared?

io_connect_t lmuGetDataPort(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;

	if (lmuDataPort) return lmuDataPort;

	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleLMUController"));

	if (!serviceObject)
	{
		printf("AppleLMU error: failed to find ambient light sensor\n");
		return 0;
	}

	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &lmuDataPort);
	IOObjectRelease(serviceObject);

	if (kr != KERN_SUCCESS)
	{
		printf("AppleLMU error: failed to open IOService object\n");
		return 0;
	}

	return lmuDataPort;
}

kern_return_t LMUClose()
{
	return IOServiceClose(lmuDataPort);
}

void getLightSensors(float *left, float *right)
{
    uint64_t inputValues[0];
	uint32_t inputCount = 0;

    uint64_t outputValues[2];
	uint32_t outputCount = 2;

	kern_return_t kr;

    kr = IOConnectCallScalarMethod(lmuGetDataPort(),
		kGetSensorReadingID,
		inputValues,
		inputCount,
		outputValues,
		&outputCount);

	if (kr != KERN_SUCCESS)
	{
		//printf("error getting light sensor values\n");
		return;
	}

	*left = outputValues[0];
	*right = outputValues[1];

	*left  /= 2000;
	*right /= 2000;
}

#include <stdio.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>

float getDisplayBrightness(void)
{
	CGDisplayErr      dErr;
	io_service_t      service;
	CGDirectDisplayID targetDisplay;

	CFStringRef key = CFSTR(kIODisplayBrightnessKey);
	float brightness = HUGE_VALF;

	targetDisplay = CGMainDisplayID();
	service = CGDisplayIOServicePort(targetDisplay);

	dErr = IODisplayGetFloatParameter(service, kNilOptions, key, &brightness);

	return brightness;
}

void setDisplayBrightness(float brightness)
{
	CGDisplayErr      dErr;
	io_service_t      service;
	CGDirectDisplayID targetDisplay;
	CFStringRef key = CFSTR(kIODisplayBrightnessKey);

	targetDisplay = CGMainDisplayID();
	service = CGDisplayIOServicePort(targetDisplay);

	dErr = IODisplaySetFloatParameter(service, kNilOptions, key, brightness);

}

// Keyboard Brightness

float getKeyboardBrightness(void)
{
	float f;
	kern_return_t kr;

	uint64_t inputCount = 1;
    uint64_t inputValues[1] = {0};

    uint32_t outputCount = 1;
    uint64_t outputValues[1];

    uint32_t out_brightness;

	kr = IOConnectCallScalarMethod(lmuGetDataPort(),
	    kGetLEDBrightnessID,
		inputValues,
		inputCount,
		outputValues,
		&outputCount);

    out_brightness = outputValues[0];

	if (kr != KERN_SUCCESS)
	{
		printf("getKeyboardBrightness() error\n");
		return 0;
	}

	f = out_brightness;
	f /= 0xfff;
	return (float)f;
}

void setKeyboardBrightness(float in)
{
	// static io_connect_t dp = 0; // shared?
	kern_return_t kr;

	uint64_t inputCount  = 2;
    uint64_t inputValues[2];
    uint64_t in_unknown = 0;
    uint64_t in_brightness = in * 0xfff;

    inputValues[0] = in_unknown;
    inputValues[1] = in_brightness;

    uint32_t outputCount = 1;
    uint64_t outputValues[1];

    uint32_t out_brightness;

	//kr = IOConnectMethodScalarIScalarO(dp, kSetLEDBrightnessID,
	kr = IOConnectCallScalarMethod(lmuGetDataPort(),
	    kSetLEDBrightnessID,
		inputValues,
		inputCount,
		outputValues,
		&outputCount);

    out_brightness = outputValues[0];

	if (kr != KERN_SUCCESS)
	{
		printf("setKeyboardBrightness() error\n");
		return;
	}
}
