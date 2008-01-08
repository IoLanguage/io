
#ifdef WIN32

#include <iphlpapi.h>

char *macAddress(void)
{
	PIP_ADAPTER_INFO adapterInfo;
	ULONG adapterInfoSize;
	DWORD errorCode;
	LPSTR errorCodeMessage = NULL;
	IoSeq *mac;

	adapterInfoSize = 0;
	errorCode = GetAdaptersInfo(NULL, &adapterInfoSize);
	if(errorCode == ERROR_BUFFER_OVERFLOW){
		adapterInfo = (IP_ADAPTER_INFO *) malloc(adapterInfoSize);
		if(adapterInfo == NULL)
			IoState_error_(IOSTATE, m, "error allocating memory while retrieving adapter info");
	}
	errorCode = GetAdaptersInfo(adapterInfo, &adapterInfoSize);
	if(errorCode != ERROR_SUCCESS){
		free(adapterInfo);
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			errorCodeMessage,
			0,
			NULL
		);
		IoState_error_(IOSTATE, m, "error retrieving network adapter info: '%s'", errorCodeMessage);
	}
	mac = IoSeq_newWithData_length_(IOSTATE, adapterInfo->Address, adapterInfo->AddressLength);
	free(adapterInfo);
	return mac;
}

#endif
