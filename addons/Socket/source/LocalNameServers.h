
//metadoc LocalNameServers copyright Steve Dekorte 2002
//metadoc LocalNameServers license BSD revised

#ifndef LOCALNAMESERVERS_DEFINED
#define LOCALNAMESERVERS_DEFINED 1

#include "List.h"

typedef struct LocalNameServers
{
	List *ips;
} LocalNameServers;

LocalNameServers *LocalNameServers_new(void);
void LocalNameServers_free(LocalNameServers *self);

void LocalNameServers_addIPAddress_(LocalNameServers *self, const char *s);

/* returns List of char * strings contianing IPs */
List *LocalNameServers_ips(LocalNameServers *self);

#endif
