
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

kern_return_t SMCClose()
{
	return IOServiceClose(conn);
}

kern_return_t SMCCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure)
{
	size_t   structureInputSize;
	size_t   structureOutputSize;
	
	structureInputSize = sizeof(SMCKeyData_t);
	structureOutputSize = sizeof(SMCKeyData_t);
	
	return IOConnectCallStructMethod( conn, index,
									 // inputStructure
									 inputStructure, structureInputSize,
									 // ouputStructure
									 outputStructure, &structureOutputSize );
	
}

kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t *val)
{
	kern_return_t result;
	SMCKeyData_t  inputStructure;
	SMCKeyData_t  outputStructure;
	
	memset(&inputStructure, 0, sizeof(SMCKeyData_t));
	memset(&outputStructure, 0, sizeof(SMCKeyData_t));
	memset(val, 0, sizeof(SMCVal_t));
	
	inputStructure.key = _strtoul(key, 4, 16);
	inputStructure.data8 = SMC_CMD_READ_KEYINFO;
	
	result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
	if (result != kIOReturnSuccess)
		return result;
	
	val->dataSize = outputStructure.keyInfo.dataSize;
	_ultostr(val->dataType, outputStructure.keyInfo.dataType);
	inputStructure.keyInfo.dataSize = val->dataSize;
	inputStructure.data8 = SMC_CMD_READ_BYTES;
	
	result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
	if (result != kIOReturnSuccess)
		return result;
	
	memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));
	
	return kIOReturnSuccess;
}
