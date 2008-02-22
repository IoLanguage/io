#ifndef KEY_TYPE_DEFINED
#define KEY_TYPE_DEFINED 1
#ifdef WIN32
	#define uint8_t unsigned char
	#define uint16_t unsigned short
	#define uint32_t unsigned long
	#define uint64_t unsigned long long
#else
   #include <stdint.h>
#endif
#endif