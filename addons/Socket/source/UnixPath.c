#include "UnixPath.h"
#include <stdlib.h>
#include <string.h>

#define PATHLEN_TO_SIZE(pathlen) ((pathlen) + sizeof(sa_family_t))
#define SIZE_TO_PATHLEN(size)    ((size)    - sizeof(sa_family_t))
#define MAXSIZE                  (sizeof(struct sockaddr_un))

UnixPath *UnixPath_new(void)
{
	UnixPath *self = io_calloc(1, sizeof(UnixPath));
	self->sockaddr = io_calloc(1, sizeof(struct sockaddr_un));
	self->size = sizeof(sa_family_t);
	self->addr = Address_newWithUnixPath(self);
	return self;
}

UnixPath *UnixPath_setUnixPath_size_(UnixPath *self,
							   struct sockaddr *address,
							   size_t size)
{
	return self;
}

void UnixPath_free(UnixPath *self)
{
	io_free(self->sockaddr);
	io_free(self);
}

// access

struct sockaddr *UnixPath_sockaddr(UnixPath *self)
{
	//printf("UnixPath_sockaddr: %s\n", self->sockaddr->sun_path);
	return (struct sockaddr *)(self->sockaddr);
}

socklen_t UnixPath_size(UnixPath *self)
{
	return self->size;
}

void UnixPath_setSize_(UnixPath *self, socklen_t size)
{
	self->size = size;
}

uint16_t UnixPath_family(UnixPath *self)
{
	return AF_UNIX;
}

void UnixPath_setPath_(UnixPath *self, char *path, size_t pathlen)
{
	self->sockaddr->sun_family = AF_UNIX;
	self->size = PATHLEN_TO_SIZE(pathlen);
	if(self->size > MAXSIZE) self->size = MAXSIZE;
	memcpy(self->sockaddr->sun_path, path, SIZE_TO_PATHLEN(self->size));
	//printf("UnixPath_setPath: %s\n", self->sockaddr->sun_path);
}

char *UnixPath_path(UnixPath *self, size_t *pathlen)
{
	//printf("UnixPath_path: %s\n", self->sockaddr->sun_path);
	*pathlen = SIZE_TO_PATHLEN(self->size);
	return self->sockaddr->sun_path;
}
