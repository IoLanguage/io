
//metadoc Common copyright Steve Dekorte 2002
//metadoc Common license BSD revised
/*metadoc Common description
You may need to add an entry for your C compiler.
*/

/*
Trick to get inlining to work with various compilers
Kudos to Daniel A. Koepke
*/

/*
 #if !defined(NS_INLINE)
 #if defined(__GNUC__)
 #define NS_INLINE static __inline__ __attribute__((always_inline))
 #elif defined(__MWERKS__) || defined(__cplusplus)
 #define NS_INLINE static inline
 #elif defined(_MSC_VER)
 #define NS_INLINE static __inline
 #elif TARGET_OS_WIN32
 #define NS_INLINE static __inline__
 #endif
 #endif
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


#if defined __XCODE__ && (TARGET_ASPEN_SIMULATOR || TARGET_OS_ASPEN)
	#define NON_EXTERN_INLINES 1
#else
	#if defined __GNUC__ && __GNUC__ >= 4
		#define NON_EXTERN_INLINES 1
	#endif
#endif
*/


#if defined(__APPLE__) 

	#ifndef NS_INLINE
		#define NS_INLINE static inline
	#endif

	#ifdef IO_IN_C_FILE
		// in .c 
		#define IO_DECLARE_INLINES
		#define IOINLINE NS_INLINE
	#else
		// in .h 
		#define IO_DECLARE_INLINES
		#define IOINLINE NS_INLINE
	#endif 	

/*		
	#include "TargetConditionals.h"


	//#if defined(__llvm__)  && 
	#if defined(__XCODE__) 
		//__GNUC__ && __GNUC__ >= 4
		//#warning inline for xcode 
		#ifdef IO_IN_C_FILE
			// in .c 
			#define IO_DECLARE_INLINES
			#define IOINLINE 
		#else
			// in .h 
			#define IO_DECLARE_INLINES
			#define IOINLINE inline
		#endif
	#else
		//#warning inline for NON-xcode 
		#ifdef IO_IN_C_FILE
			// in .c 
			#define IO_DECLARE_INLINES
			#define IOINLINE inline
		#else
			// in .h 
			#define IO_DECLARE_INLINES
			#define IOINLINE extern inline
		#endif 

	#endif
	*/
	
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
	
#elif defined(__linux__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)

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
