//metadoc GLIncludes copyright Steve Dekorte 2002
//metadoc GLIncludes license BSD revised

#ifndef GLINCLUDES_DEFINED
#define GLINCLUDES_DEFINED 1

#if defined(__APPLE__) || defined(MACOSX)
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/gl.h>
#else
	#ifdef _WIN32
		#include <windows.h>
		#include <GL/gl.h>
		#include <GL/glut.h>
		#include <GL/glu.h>
		#define GL_CONSTANT_COLOR                 0x8001
		#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
		#define GL_CONSTANT_ALPHA                 0x8003
		#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
		#define GL_BLEND_COLOR                    0x8005
	#else
		#include <GL/glut.h>
		#include <GL/glu.h>
		#include <GL/gl.h>
	#endif
#endif


#endif
