#include "UnixPath.h"
#include <stdlib.h>
#include <string.h>

UnixPath *UnixPath_new(void)
{
	UnixPath *self = io_calloc(1, sizeof(UnixPath));
	self->sockaddr = io_calloc(1, sizeof(struct sockaddr_un));
	self->size = sizeof(struct sockaddr_un);
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

void UnixPath_setPath_(UnixPath *self, char *path)
{
	self->sockaddr->sun_family = AF_UNIX;
	strncpy(self->sockaddr->sun_path, path, UNIX_PATH_MAX);
	//printf("UnixPath_setPath: %s\n", self->sockaddr->sun_path);
}

char *UnixPath_path(UnixPath *self)
{
	//printf("UnixPath_path: %s\n", self->sockaddr->sun_path);
	return self->sockaddr->sun_path;
}
