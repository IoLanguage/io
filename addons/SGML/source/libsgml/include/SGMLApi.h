#ifndef SGML_API_H
#define SGML_API_H
#ifdef WIN32
#ifdef LIBSGML_EXPORTS
#define SGML_API __declspec(dllexport)
#else
#define SGML_API __declspec(dllexport)
#endif
#else
#define SGML_API
#endif
#endif