
//metadoc DNS copyright Steve Dekorte 2002
//metadoc DNS license BSD revised
//metadoc DNS category Networking
/*metadoc DNS description
Utility methods related to Domain Name Service lookups. 
*/
//metadoc DNS category Networking

#include "IoDNS.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoList.h"
#include <ctype.h>

#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#include <netinet/in.h>
#else
typedef unsigned short ushort;
#endif

#include "UArray.h"

struct question
{
	char *name;
	ushort type;
	ushort class;
};

struct rr
{
	char *name;
	ushort type;
	ushort class;
	unsigned ttl;
	ushort rdlength;
	unsigned char *rdata;
};

struct msg
{
	ushort id;
	ushort flags;
	ushort qdcount;
	ushort ancount;
	ushort nscount;
	ushort arcount;
	struct question *qd;
	struct rr *an;
	struct rr *ns;
	struct rr *ar;
	union {
		struct question q;
		struct rr r;
		char names[2020];
	} data;
};

/*
typedef struct DNSQuery
{
	List *hosts;
	void *server;
	time_t endt;	// when does the query expire
	time_t nextt;	// when to resend the query
	int errors;
	int pktlen; // how much is used of pkt
	ushort id;  // the id of this query
	unsigned char pkt[512]; // the assembled query
} DNSQuery;
*/

// dns query packet assembly ------------------------------------

static void qname(char *s)
{
	char *dot = strchr(s + 1, '.');

	if (dot)
	{
		*s = dot - (s + 1);
		qname(dot);
	}
	else
	{
		*s = strlen(s) - 1;
	}
}

int assemble(unsigned char *packetData, uint32_t packetDataSize, uint16_t id, const char *hostName)
{
	unsigned char *p = packetData;

	memset(p, 0, packetDataSize);

	if (12 + strlen(hostName) + 1 + 4 >= packetDataSize)
	{
		return -1;
	}

	/*
	 DNS HEADER FORMAT

	 OCTET 1,2       ID
	 OCTET 3,4       QR(1 bit) + OPCODE(4 bit)+ AA(1 bit) + TC(1 bit) + RD(1 bit)+ RA(1 bit) +
			Z(3 bit) + RCODE(4 bit)
	 OCTET 5,6       QDCOUNT
	 OCTET 7,8       ANCOUNT
	 OCTET 9,10      NSCOUNT
	 OCTET 11,12     ARCOUNT
	 */

	// set first two bytes to ID

	*p = id >> 8;
	p ++;

	*p = id;
	p ++;

	memcpy(p, "\1\0\0\1\0\0\0\0\0\0", 10);
	p += 10;

	/*
	 QUESTION FORMAT
	 OCTET 1,2,Én    QNAME
	 */

	*p = '.';
	strcpy((char *)p + 1, hostName);


	// replace dots with length until dot or end

	qname((char *)p);

	p += strlen(hostName) + 1;

	*p = 0;
	p ++;

	/*
	 OCTET n+1,n+2   QTYPE
	 OCTET n+3,n+4   QCLASS
	 */
	/* QTYPEs: A, NS, MX, SOA, CNAME, HINFO  */
	memcpy(p, "\0\1\0\1", 4); /* QTYPE=1 (Host IPAddress),  QCLASS=1 (Internet). */

	p += 4;

	return p - packetData;
}

// binding -------------------------------

IoObject *IoObject_dnsQueryPacketForHostName(IoObject *self, IoObject *locals, IoMessage *m)
{
/*doc DNS dnsQueryPacketForHostName(hostNameSeq)
Assembles a DNS query packet for the given host name and returns it in a Sequence.
*/
	IoSeq *hostName = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoSeq *packet = IoSeq_new(IOSTATE);
	int packetLength;
	char *host = CSTRING(hostName);

	IoSeq_rawSetSize_(packet, 512);
	packetLength = assemble(IoSeq_rawBytes(packet), 512, 0, host);
	IoSeq_rawSetSize_(packet, packetLength);

	return packet;
}

// dns response packet disassembly -------

static int get16(unsigned char *p)
{
	return p[0] * 256 + p[1];
}

static int get32(unsigned char *p)
{
	return ((p[0] * 256 + p[1]) * 256 + p[2]) * 256 + p[3];
}

static unsigned char *getdomain(unsigned char *base, int len, unsigned char *p, char **_dst, char *dend)
{
	unsigned char *end = base + len;
	char *dst = *_dst;
	unsigned char *final = NULL;
	int n;

	for (;;)
	{
		if (p >= end)
		{
			return NULL;
		}

		n = *p++;

		if (n >= 192)
		{
			if (p >= end)
			{
				return NULL;
			}

			end = p-1;
			p = base + 256*n + *p - 0xc000;

			if (!final)
			{
				final = end+2;
			}
			continue;
		}

		if (n >= 64)
		{
			return NULL;
		}

		if (dst + n + 1 > dend)
		{
			return NULL;
		}

		if (n == 0)
		{
			break;
		}

		if (p + n > end)
		{
			return NULL;
		}

		if (dst != *_dst)
		{
			*dst++ = '.';
		}

		memcpy(dst, p, n);
		dst += n;
		p += n;
	}

	*dst++ = '\0';
	*_dst = dst;

	return final ? final : p;
}

static int disassemble(unsigned char *buf, int len, struct msg *msg)
{
	struct question *qd;
	struct rr *rr;
	unsigned char *p;
	char *names;
	int i;

	if (len < 12)
	{
		return -1;
	}

	msg->id = get16(buf);
	msg->flags = get16(buf+2);
	msg->qdcount = get16(buf+4);
	msg->ancount = get16(buf+6);
	msg->nscount = get16(buf+8);
	msg->arcount = get16(buf+10);
	msg->qd = &msg->data.q;
	msg->an = (void*)(msg->qd + msg->qdcount);
	msg->ns = msg->an + msg->ancount;
	msg->ar = msg->ns + msg->nscount;
	names = (void*)(msg->ar + msg->arcount);
	p = buf + 12;
	i = msg->qdcount;

	for (qd = msg->qd; i --; ++qd)
	{
		qd->name = names;
		p = getdomain(buf, len - 4, p, &names, (char*)(msg + 1));

		if (!p)
		{
			return -2;
		}

		qd->type = get16(p);
		qd->class = get16(p + 2);
		p += 4;
	}

	i = msg->ancount + msg->nscount + msg->arcount;

	for (rr = msg->an; i --; ++rr)
	{
		rr->name = names;
		p = getdomain(buf, len - 10, p, &names, (char*)(msg + 1));

		if (!p)
		{
			return -3;
		}

		rr->type     = get16(p);
		rr->class    = get16(p + 2);
		rr->ttl      = htonl(get32(p + 4));
		rr->rdlength = get16(p + 8);
		rr->rdata    = p + 10;
		p += 10 + rr->rdlength;
	}

	if (p > buf+len)
	{
		return -4;
	}

	return 0;
}

// binding -------------------------------

IoObject *IoObject_hostNameAndIPforDNSResponsePacket(IoObject *self, IoObject *locals, IoMessage *m)
{
/*doc DNS hostNameAndIPforDNSResponsePacket(dnsResponsePacketSeq)
Dissasembles the given dnsResponsePacketSeq and returns a list object containing
the hostName and IP or an error string on error.
*/
	IoSeq *packet = IoMessage_locals_seqArgAt_(m, locals, 0);
	IoList *list = IoList_new(IOSTATE);

	struct rr *rr;
	struct msg msg;

	unsigned char *buf = IoSeq_rawBytes(packet);
	int len = IoSeq_rawSize(packet);

	if (disassemble(buf, len, &msg))
	{
		return DNSERROR("Malformed packet");
	}

	if ((msg.flags & 0xf980) != 0x8180) // QR|RD|RA
	{
		return DNSERROR("Wrong flags (not a reply etc)");
	}

	if (msg.qdcount != 1 || msg.qd[0].type != 1 || msg.qd[0].class != 1)
	{
		return DNSERROR("Bad packet from server");
	}

	// hostName
	IoList_rawAppend_(list, IoState_symbolWithCString_(IOSTATE, msg.qd[0].name));

	// printf("answer for %s\n", Host_name(h));

	switch (msg.flags & 0x000f)
	{
		case 0:	// noerror
			//h->expire = io_time + IOSEC(DNS_NOADDR_EXPIRE,1);
			for (rr = msg.an; rr < msg.an + msg.ancount; ++rr)
			{
				if (rr->type == 1 && rr->class == 1 && rr->rdlength == 4)
				{
					/*
					 if (rr->ttl > 0 && rr->ttl <= 24*60*60)
					 h->expire = io_time + IOSEC(rr->ttl,1);
					 */
				{
					struct in_addr addr;
					char *ip;
					addr.s_addr = htonl(get32(rr->rdata));
					ip = (char *)inet_ntoa(addr);
					IoList_rawAppend_(list, IoState_symbolWithCString_(IOSTATE, ip));
				}
				}
			}
			break;
		case 1: // formerr
			return DNSERROR("formerr");
			break;
		case 2: // servfail (temporary)
			//h->query->nextt = io_time;
			return DNSERROR("servfail (temporary)");
			break;
		case 3:	// nxdomain
			return DNSERROR("nxdomain");
			// h->expire = io_time + IOSEC(DNS_NXDOM_EXPIRE,1);
			break;
		case 4: // notimpl
			return DNSERROR("notimpl");
			break;
		case 5: // refused
			DNSERROR("refused");
			break;
		default:
			DNSERROR("bad server");
			break;
	}

	return list;
}

#include "LocalNameServers.h"

IoObject *IoObject_localNameServersIPs(IoObject *self, IoObject *locals, IoMessage *m)
{
/*doc DNS localNameServersIPs
Returns a list of local name server IPs as a list of Sequences. Works on OSX, Unix, Windows.
*/
	IoList *list = IoList_new(IOSTATE);
	LocalNameServers *lns = LocalNameServers_new();
	List *ips = LocalNameServers_ips(lns);

	LIST_FOREACH(ips, i, ip,
		IoList_rawAppend_(list, IoState_symbolWithCString_(IOSTATE, (char *)ip));
	);

	LocalNameServers_free(lns);
	return list;
}

IoObject *IoDNS_proto(void *state)
{
	IoObject *self = IoState_doCString_(state, "Object clone");

	IoObject_addMethod_(self, IOSYMBOL("dnsQueryPacketForHostName"),
					IoObject_dnsQueryPacketForHostName);

	IoObject_addMethod_(self, IOSYMBOL("hostNameAndIPforDNSResponsePacket"),
					IoObject_hostNameAndIPforDNSResponsePacket);

	IoObject_addMethod_(self, IOSYMBOL("localNameServersIPs"),
					IoObject_localNameServersIPs);

	IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("DNS"));

	return self;
}
