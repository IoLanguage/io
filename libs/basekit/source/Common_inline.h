
//metadoc Common copyright Steve Dekorte 2002
//metadoc Common license BSD revised
/*metadoc Common description
You may need to add an entry for your C compiler.
*/

/*
Trick to get inlining to work with various compilers
Kudos to Daniel A. Koepke
*/

#undef IO_DECLARE_INLINES
#undef IOINLINE

/*
#if defined(__cplusplus)
	#ifdef IO_IN_C_FILE
	#else
		#define IO_DECLARE_INLINES
		#define IOINLINE extern inline
	#endif
#else
*/

/*
#if defined(__APPLE__)
	#include "TargetConditionals.h"
#endif

#if defined __XCODE__ && (TARGET_ASPEN_SIMULATOR || TARGET_OS_ASPEN)
	#define NON_EXTERN_INLINES 1
#else
	#if defined __GNUC__ && __GNUC__ >= 4
		#define NON_EXTERN_INLINES 1
	#endif
#endif
*/


#if defined(__APPLE__)
	#ifdef IO_IN_C_FILE
		// in .c 
		#define IO_DECLARE_INLINES
		#define IOINLINE 
	#else
		// in .h 
		#define IO_DECLARE_INLINES
		#define IOINLINE inline
	#endif 
#elif defined(__MINGW32__)
	#ifdef IO_IN_C_FILE
		// in .c 
		#define IO_DECLARE_INLINES
		#define IOINLINE inline
	#else
		// in .h 
		#define IO_DECLARE_INLINES
		#define IOINLINE static inline
	#endif 
#elif defined(__linux__)
	#ifdef IO_IN_C_FILE
		// in .c 
		#define IO_DECLARE_INLINES
		#define IOINLINE inline
	#else
		// in .h 
		#define IO_DECLARE_INLINES
		#define IOINLINE extern inline
	#endif 
#else
	#ifdef IO_IN_C_FILE
		// in .c 
		#define IO_DECLARE_INLINES
		#define IOINLINE 
	#else
		// in .h 
		#define IO_DECLARE_INLINES
		#define IOINLINE inline
	#endif 
#endif
