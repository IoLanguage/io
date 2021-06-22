/*
 * freeglut_overlay.c
 *
 * Overlay management functions (as defined by GLUT API)
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Thu Dec 16 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../include/GL/freeglut.h"
#include "freeglut_internal.h"

/*
 * NOTE: functions declared in this file probably will not be implemented.
 */

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

void  glutEstablishOverlay( void )             { /* Not implemented */ }
void  glutRemoveOverlay( void )                { /* Not implemented */ }
void  glutUseLayer( GLenum layer )             { /* Not implemented */ }
void  glutPostOverlayRedisplay( void )         { /* Not implemented */ }
void  glutPostWindowOverlayRedisplay( int ID ) { /* Not implemented */ }
void  glutShowOverlay( void )                  { /* Not implemented */ }
void  glutHideOverlay( void )                  { /* Not implemented */ }

/*** END OF FILE ***/
