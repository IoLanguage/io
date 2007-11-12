#ifndef IO_BOX_H_
#define IO_BOX_H_

#if defined(WIN32)
#if defined(BUILDING_BOX_ADDON)
#define IOBOX_API __declspec(dllexport)
#else
#define IOBOX_API __declspec(dllimport)
#endif

#else
#define IOBOX_API
#endif

#endif
