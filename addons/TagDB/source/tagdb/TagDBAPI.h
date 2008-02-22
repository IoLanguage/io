#ifndef TAGDB_API_H
#define TAGDB_API_H
#ifdef WIN32
#ifdef LIBTAGDB_EXPORTS
#define TAGDB_API __declspec(dllexport)
#else
#define TAGDB_API __declspec(dllexport)
#endif
#else
#define TAGDB_API
#endif
#endif