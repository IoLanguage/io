#ifndef IO_IMAGE_API_H
#define IO_IMAGE_API_H

#ifdef WIN32
#ifdef BUILDING_IMAGE_ADDON
// we are building the Image addon
#define IOIMAGE_API __declspec(dllexport)
#else
// we link against the Vector addon
#define IOIMAGE_API __declspec(dllimport)
#endif
#else // then not on Windows just define this to nothing
#define IOIMAGE_API
#endif

#endif
