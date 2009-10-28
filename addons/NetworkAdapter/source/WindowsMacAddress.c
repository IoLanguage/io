
int __WindowsMacAddress(int a) // just to make ranlib happy
{
	return a + 1;
}

#ifdef WIN32

#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#include <iphlpapi.h>


char *String_asHex(char *in, size_t len) // caller must free result
{
	char *out = (char *) malloc(len * 2 + 1);
	int i;

	for(i = 0; i < len; i++)
	{
		BYTE c = in[i];

		if (c < 16)
		{
			sprintf(out + (i * 2), "0%x", c);
		}
		else
		{
			sprintf(out + (i * 2), "%x", c);
		}
	}

	out[len * 2] = 0x0;
	return out;
}

char *macAddress(void)
{
	IP_ADAPTER_INFO *adapterInfo;
	ULONG adapterInfoSize = 0;

	if (GetAdaptersInfo(NULL, &adapterInfoSize) != ERROR_SUCCESS)
	{
		adapterInfo = (IP_ADAPTER_INFO *) malloc(adapterInfoSize);
	}

	if (GetAdaptersInfo(adapterInfo, &adapterInfoSize) != ERROR_SUCCESS)
	{
		return NULL;
	}

	{
		char *mac = String_asHex(adapterInfo->Address, adapterInfo->AddressLength);
		free(adapterInfo);
		return mac;
	}
}

#endif