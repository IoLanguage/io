
//metadoc LocalNameServers copyright Steve Dekorte 2002
//metadoc LocalNameServers license BSD revised
//metadoc LocalNameServers category Networking
/*metadoc LocalNameServers description
Windows code by Mike Austin.
*/
//metadoc LocalNameServers category Networking

#include "LocalNameServers.h"
#include <ctype.h>

void LocalNameServers_findIps(LocalNameServers *self);

LocalNameServers *LocalNameServers_new(void)
{
	LocalNameServers *self = (LocalNameServers *)io_calloc(1, sizeof(LocalNameServers));
	return self;
}

void LocalNameServers_free(LocalNameServers *self)
{
	if (self->ips)
	{
		List_do_(self->ips, (ListDoCallback *)io_free);
	}

	List_free(self->ips);
	io_free(self);
}

List *LocalNameServers_ips(LocalNameServers *self)
{
	if (!self->ips)
	{
		self->ips = List_new();
		LocalNameServers_findIps(self);
	}

	return self->ips;
}

void LocalNameServers_addIPAddress_(LocalNameServers *self, const char *s)
{
	char *newIPAddress = strcpy(io_malloc(strlen(s) + 1), s);
	List_append_(self->ips, newIPAddress);
}

// --- generic -----------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "UArray.h"

static char *stringDeleteHashComment(char *s)
{
	char *commentBeginning = strchr(s, '#');

	if(!commentBeginning)
	{
		commentBeginning = s + strlen(s);
	}

	while(!isdigit(*commentBeginning))
	{
		*commentBeginning-- = '\0';
	}

	return s;
}

static char *lastWhiteSpaceInString(char *s)
{
	char *lastSpace = strrchr(s, ' ');
	char *lastTab = strrchr(s, '\t');
	char *lastWhiteSpace = lastSpace > lastTab ? lastSpace : lastTab;
	return lastWhiteSpace;
}

static char *local_strdup(char *s) // because OSXs is buggy
{
	return strcpy(io_malloc(strlen(s)+1), s);
}

void LocalNameServers_findIpsViaResolveConf(LocalNameServers *self)
{
	FILE *fp = fopen("/etc/resolv.conf", "r");

	if (fp)
	{
		UArray *ba = UArray_new();

		while (UArray_readLineFromCStream_(ba, fp))
		{
			char *line = (char *)UArray_bytes(ba);

			/*printf("line = %s\n", line);*/
			if (strstr(line, "nameserver") == line)
			{
				char *ip;
				char *s = local_strdup(line);

				stringDeleteHashComment(s);
				ip = lastWhiteSpaceInString(s) + 1;

				if (*ip)
				{
					//printf("LocalNameServers_findIps() found ip '%s'\n", ip);
					LocalNameServers_addIPAddress_(self, ip);
				}

				io_free(s);
			}
			UArray_setSize_(ba, 0);
		}
		UArray_free(ba);
	}
}


#if defined(WIN32) || defined(__CYGWIN__)

// Windows -----------------------------------------

#include <winsock2.h>
#include <iphlpapi.h>

void LocalNameServers_findIps(LocalNameServers *self)
{
	FIXED_INFO info[16];
	ULONG len = sizeof(FIXED_INFO) * 16;

	if (GetNetworkParams(info, &len) != ERROR_SUCCESS)
	{
		printf("LocalNameServers error: GetNetworkParams() failed");
		/*exit(1);*/
		return;
	}


	{
		IP_ADDR_STRING *addr = &info->DnsServerList;

		while (addr)
		{
			LocalNameServers_addIPAddress_(self, addr->IpAddress.String);
			/*printf("%s\n", addr->IpIPAddress.String);*/
			addr = addr->Next;
		}
	}
}

#elif defined(__APPLE__)

// OSX ----------------------------------------

void LocalNameServers_findIps(LocalNameServers *self)
{
        char *path = NULL;
	mkstemp(path);
	char *command = io_malloc(1024);
	char *answerBuffer = io_calloc(1, 1024);
	char *answer = answerBuffer;
	FILE *fp;

	sprintf(command, "dig | grep SERVER: > %s", path);

	system(command);

	fp = fopen(path, "r");
	fread(answer, 1, 1024, fp);
	fclose(fp);
	remove(path);

	if (strlen(answer) < strlen(";; SERVER: "))
	{
		//printf("LocalNameServers warning: unable to find nameservers using 'dig | grep SERVER:'\nParsing resolve.conf instead.");
		LocalNameServers_findIpsViaResolveConf(self);
		goto done;
	}

	answer = answer + strlen(";; SERVER: ");

	{
		char *s = strstr(answer, "#");
		if(s) s[0] = 0;
	}

	LocalNameServers_addIPAddress_(self, answer);

	done:
	io_free(command);
	io_free(answerBuffer);
}

#else

// Unix ----------------------------------------


void LocalNameServers_findIps(LocalNameServers *self)
{
	LocalNameServers_findIpsViaResolveConf(self);
}

#endif
