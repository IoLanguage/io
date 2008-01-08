
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


#include <mach/mach.h>
//#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

static io_connect_t dataPort = 0; // shared?

io_connect_t getDataPort(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;

	if (dataPort) return dataPort;

	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleLMUController"));

	if (!serviceObject)
	{
		printf("getLightSensors() error: failed to find ambient light sensor\n");
		return 0;
	}

	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &dataPort);
	IOObjectRelease(serviceObject);

	if (kr != KERN_SUCCESS)
	{
		printf("getLightSensors() error: failed to open IoService object\n");
		return 0;
	}

	return dataPort;
}

void getLightSensors(float *left, float *right)
{
	IOItemCount scalarInputCount = 0;
	IOItemCount scalarOutputCount = 2;
	kern_return_t kr;
	SInt32 l = 0, r = 0;

	kr = IOConnectMethodScalarIScalarO(getDataPort(),
		kGetSensorReadingID,
		scalarInputCount,
		scalarOutputCount,
		&l,
		&r);

	if (kr != KERN_SUCCESS)
	{
		//printf("error getting light sensor values\n");
		return;
	}

	*left = l;
	*right = r;

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
	IOItemCount   scalarInputCount  = 1;
	IOItemCount   scalarOutputCount = 1;
	SInt32        in_unknown = 0, out_brightness;

	kr = IOConnectMethodScalarIScalarO(getDataPort(), kGetLEDBrightnessID,
		scalarInputCount,
		scalarOutputCount,
		in_unknown,
		&out_brightness);

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
	static io_connect_t dp = 0; // shared?
	kern_return_t kr;
	IOItemCount   scalarInputCount  = 2;
	IOItemCount   scalarOutputCount = 1;
	SInt32        in_unknown = 0, in_brightness, out_brightness;

	in_brightness = in * 0xfff;

	//kr = IOConnectMethodScalarIScalarO(getDataPort(), kSetLEDBrightnessID,
	kr = IOConnectMethodScalarIScalarO(dp, kSetLEDBrightnessID,
		scalarInputCount,
		scalarOutputCount,
		in_unknown,
		in_brightness,
		&out_brightness);

	if (kr != KERN_SUCCESS)
	{
		printf("setKeyboardBrightness() error\n");
		return;
	}
}

