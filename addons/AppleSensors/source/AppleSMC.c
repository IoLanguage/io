
#include "AppleSMC.h"

static io_connect_t smcDataPort = 0; // shared?

UInt32 _strtoul(char *str, int size, int base)
{
	UInt32 total = 0;
	int i;
	
	for (i = 0; i < size; i++)
	{
		if (base == 16)
			total += str[i] << (size - 1 - i) * 8;
		else
			total += (unsigned char) (str[i] << (size - 1 - i) * 8);
	}
	
	return total;
}

float _strtof(unsigned char *str, int size, int e)
{
	float total = 0;
	int i;
	
	for (i = 0; i < size; i++)
	{
		if (i == (size - 1))
			total += (str[i] & 0xff) >> e;
		else
			total += str[i] << (size - 1 - i) * (8 - e);
	}
	
	total += (str[size-1] & 0x03) * 0.25;
	
	return total;
}

void _ultostr(char *str, UInt32 val)
{
	str[0] = '\0';
	sprintf(str, "%c%c%c%c",
			(unsigned int) val >> 24,
			(unsigned int) val >> 16,
			(unsigned int) val >> 8,
			(unsigned int) val);
}

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
	return IOServiceClose(smcDataPort);
}

kern_return_t SMCCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure)
{
	size_t   structureInputSize;
	size_t   structureOutputSize;
	
	structureInputSize = sizeof(SMCKeyData_t);
	structureOutputSize = sizeof(SMCKeyData_t);
	
	return IOConnectCallStructMethod( smcGetDataPort(), index,
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

bool representValue(SMCVal_t value, char* str)
{
	if (value.dataSize > 0) {
		if ((strcmp(value.dataType, DATATYPE_UINT8) == 0) ||
			(strcmp(value.dataType, DATATYPE_UINT16) == 0) ||
			(strcmp(value.dataType, DATATYPE_UINT32) == 0)) {
			UInt32 uint= _strtoul(value.bytes, value.dataSize, 10);
			snprintf(str, 15, "%u ", (unsigned int)uint);
		}
		else if (strcmp(value.dataType, DATATYPE_FP1F) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.5f ", ntohs(*(UInt16*)value.bytes) / 32768.0);
		else if (strcmp(value.dataType, DATATYPE_FP4C) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.5f ", ntohs(*(UInt16*)value.bytes) / 4096.0);
		else if (strcmp(value.dataType, DATATYPE_FP5B) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.5f ", ntohs(*(UInt16*)value.bytes) / 2048.0);
		else if (strcmp(value.dataType, DATATYPE_FP6A) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.4f ", ntohs(*(UInt16*)value.bytes) / 1024.0);
		else if (strcmp(value.dataType, DATATYPE_FP79) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.4f ", ntohs(*(UInt16*)value.bytes) / 512.0);
		else if (strcmp(value.dataType, DATATYPE_FP88) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.3f ", ntohs(*(UInt16*)value.bytes) / 256.0);
		else if (strcmp(value.dataType, DATATYPE_FPA6) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.2f ", ntohs(*(UInt16*)value.bytes) / 64.0);
		else if (strcmp(value.dataType, DATATYPE_FPC4) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.2f ", ntohs(*(UInt16*)value.bytes) / 16.0);
		else if (strcmp(value.dataType, DATATYPE_FPE2) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.2f ", ntohs(*(UInt16*)value.bytes) / 4.0);
		else if (strcmp(value.dataType, DATATYPE_SP1E) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.5f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 16384.0);
		else if (strcmp(value.dataType, DATATYPE_SP3C) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.5f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 4096.0);
		else if (strcmp(value.dataType, DATATYPE_SP4B) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.4f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 2048.0);
		else if (strcmp(value.dataType, DATATYPE_SP5A) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.4f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 1024.0);
		else if (strcmp(value.dataType, DATATYPE_SP69) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.3f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 512.0);
		else if (strcmp(value.dataType, DATATYPE_SP78) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.3f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 256.0);
		else if (strcmp(value.dataType, DATATYPE_SP87) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.3f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 128.0);
		else if (strcmp(value.dataType, DATATYPE_SP96) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.2f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 64.0);
		else if (strcmp(value.dataType, DATATYPE_SPB4) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.2f ", ((SInt16)ntohs(*(UInt16*)value.bytes)) / 16.0);
		else if (strcmp(value.dataType, DATATYPE_SPF0) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.0f ", (float)ntohs(*(UInt16*)value.bytes));
		else if (strcmp(value.dataType, DATATYPE_SI16) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%d ", ntohs(*(SInt16*)value.bytes));
		else if (strcmp(value.dataType, DATATYPE_SI8) == 0 && value.dataSize == 1)
			snprintf(str, 15, "%d ", (signed char)*value.bytes);
		else if (strcmp(value.dataType, DATATYPE_PWM) == 0 && value.dataSize == 2)
			snprintf(str, 15, "%.1f%% ", ntohs(*(UInt16*)value.bytes) * 100 / 65536.0);
		else if (strcmp(value.dataType, DATATYPE_CHARSTAR) == 0)
			snprintf(str, 15, "%s ", value.bytes);
		else if (strcmp(value.dataType, DATATYPE_FLAG) == 0)
			snprintf(str, 15, "%s ", value.bytes[0] ? "TRUE" : "FALSE");
		else {
			int i;
			char tempAb[64];
			for (i = 0; i < value.dataSize; i++) {
				snprintf(tempAb+strlen(tempAb), 8, "%02x ", (unsigned char) value.bytes[i]);
			}
			snprintf(str, 15, "%s ", tempAb);
		}
		return TRUE;
	}
	return FALSE;
}

SMCVal_t createEmptyValue() {
	SMCVal_t newVal;
	memset(&newVal, 0, sizeof(newVal));
	return newVal;
}
