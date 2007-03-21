#ifndef IO_VECTOR_H_
#define IO_VECTOR_H_

#if defined(WIN32)
#if defined(BUILDING_VECTOR_ADDON)
#define IOVECTOR_API __declspec(dllexport)
#else
#define IOVECTOR_API __declspec(dllimport)
#endif

#else
#define IOVECTOR_API
#endif

#endif
