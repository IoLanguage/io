
#include "AppleSMC.h"

static io_connect_t smcDataPort = 0; // shared?

io_connect_t smcGetDataPort(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;

	if (smcDataPort) return smcDataPort;

	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleSMC"));

	if (!serviceObject)
	{
		printf("AppleSMC error: failed to find SMC unit\n");
		return 0;
	}

	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &smcDataPort);
	IOObjectRelease(serviceObject);

	if (kr != KERN_SUCCESS)
	{
		printf("AppleSMC error: failed to open IOService object\n");
		return 0;
	}

	return smcDataPort;
}
