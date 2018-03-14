#ifndef IO_RANGE_H_
#define IO_RANGE_H_

#if defined(WIN32)
#if defined(BUILDING_RANGE_ADDON)
#define IORANGE_API __declspec(dllexport)
#else
#define IORANGE_API __declspec(dllimport)
#endif

#else
#define IORANGE_API
#endif

#endif
