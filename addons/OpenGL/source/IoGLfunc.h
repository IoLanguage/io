/*
 *
 * GL Functions
 *
 */

typedef struct t_ioGL_funcTable {
	char		*name;
	IoObject*	(*func)(IoObject*,IoObject*,IoMessage*);
} t_ioGL_funcTable;


t_ioGL_funcTable ioGL_funcTable[] = {

	/* OpenGL 1.1 without Extensions */

	{"glAccum",IoGL_glAccum},
	{"glAlphaFunc",IoGL_glAlphaFunc},
	{"glAreTexturesResident",IoGL_glAreTexturesResident},
	{"glArrayElement",IoGL_glArrayElement},
	{"glBegin",IoGL_glBegin},
	{"glBindTexture",IoGL_glBindTexture},
	{"glBitmap",IoGL_glBitmap},
	{"glBlendFunc",IoGL_glBlendFunc},
	{"glCallList",IoGL_glCallList},
	{"glClearAccum",IoGL_glAccum},
	{"glClearColor",IoGL_glClearColor},
	{"glClearDepth",IoGL_glClearDepth},
	{"glClearIndex",IoGL_glClearIndex},
	{"glClear",IoGL_glClear},
	{"glClearStencil",IoGL_glClearStencil},
	{"glClipPlane",IoGL_glClipPlane},
	{"glColor3b",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3bv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor3d",IoGL_glColor3d},
	{"glColor3dv",IoGL_glColor3dv},
	{"glColor3f",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3fv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor3i",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3iv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor3ub",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3ubv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor3ui",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3uiv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor3us",IoGL_glColor3d}, /*  map to 3d function */
	{"glColor3usv",IoGL_glColor3dv}, /*  map to 3dv function */
	{"glColor4b",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4bv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColor4d",IoGL_glColor4d},
	{"glColor4dv",IoGL_glColor4dv},
	{"glColor4f",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4fv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColor4i",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4iv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColor4ub",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4ubv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColor4ui",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4uiv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColor4us",IoGL_glColor4d}, /*  map to 4d function */
	{"glColor4usv",IoGL_glColor4dv}, /*  map to 4dv function */
	{"glColorMask",IoGL_glColorMask},
	{"glColorMaterial",IoGL_glColorMaterial},
	{"glColorPointer",IoGL_glColorPointer},
	{"glCopyPixels",IoGL_glCopyPixels},
	{"glCopyTexImage1D",IoGL_glCopyTexImage1D},
	{"glCopyTexImage2D",IoGL_glCopyTexImage2D},
	{"glCopyTexSubImage1D",IoGL_glCopyTexSubImage1D},
	{"glCopyTexSubImage2D",IoGL_glCopyTexSubImage2D},
	{"glCullFace",IoGL_glCullFace},
	{"glDeleteLists",IoGL_glDeleteLists},
	{"glDeleteTextures",IoGL_glDeleteTextures},
	{"glDepthFunc",IoGL_glDepthFunc},
	{"glDepthMask",IoGL_glDepthMask},
	{"glDepthRange",IoGL_glDepthRange},
	{"glDisable",IoGL_glDisable},
	{"glDrawArrays",IoGL_glDrawArrays},
	{"glDrawBuffer",IoGL_glDrawBuffer},
	{"glDrawElements",IoGL_glDrawElements},
	{"glDrawPixels",IoGL_glDrawPixels},
	{"glEdgeFlag",IoGL_glEdgeFlag},
	{"glEdgeFlagPointer",IoGL_glEdgeFlagPointer},
	{"glEnable",IoGL_glEnable},
	{"glEnd",IoGL_glEnd},
	{"glEndList",IoGL_glEndList},
	{"glEvalCoord1d",IoGL_glEvalCoord1d},
	{"glEvalCoord1dv",IoGL_glEvalCoord1dv},
	{"glEvalCoord1f",IoGL_glEvalCoord1d}, /*  map to 1d function */
	{"glEvalCoord1fv",IoGL_glEvalCoord1dv}, /*  map to 1dv function */
	{"glEvalCoord2d",IoGL_glEvalCoord2d},
	{"glEvalCoord2dv",IoGL_glEvalCoord2dv},
	{"glEvalCoord2f",IoGL_glEvalCoord2d}, /*  map to 2d function */
	{"glEvalCoord2fv",IoGL_glEvalCoord2dv}, /*  map to 2dv function */
	{"glEvalMesh1",IoGL_glEvalMesh1},
	{"glEvalMesh2",IoGL_glEvalMesh2},
	{"glEvalPoint1",IoGL_glEvalPoint1},
	{"glEvalPoint2",IoGL_glEvalPoint2},
	{"glFeedbackBuffer",IoGL_glFeedbackBuffer},
	{"glFinish",IoGL_glFinish},
	{"glFlush",IoGL_glFlush},
	{"glFogf",IoGL_glFogf},
	{"glFogfv",IoGL_glFogfv},
	{"glFogi",IoGL_glFogf}, /*  map to f function */
	{"glFogiv",IoGL_glFogfv}, /*  map to fv function */
	{"glFrontFace",IoGL_glFrontFace},
	{"glFrustum",IoGL_glFrustum},
	{"glGenLists",IoGL_glGenLists},
	{"glGenTextures",IoGL_glGenTextures},
	{"glGetBooleanv",IoGL_glGetDoublev}, /*  map to d function */
	{"glGetClipPlane",IoGL_glGetClipPlane},
	{"glGetDoublev",IoGL_glGetDoublev},
	{"glGetError",IoGL_glGetError},
	{"glGetFloatv",IoGL_glGetDoublev}, /*  map to d function */
	{"glGetIntegerv",IoGL_glGetDoublev}, /*  map to d function */
	{"glGetLightfv",IoGL_glGetLightfv},
	{"glGetLightiv",IoGL_glGetLightfv}, /*  map to fv function */
	{"glGetMapdv",IoGL_glGetMapdv},
	{"glGetMapfv",IoGL_glGetMapdv}, /*  map to dv function */
	{"glGetMapiv",IoGL_glGetMapdv}, /*  map to dv function */
	{"glGetMaterialfv",IoGL_glGetMaterialfv},
	{"glGetMaterialiv",IoGL_glGetMaterialfv}, /*  map to fv function */
	{"glGetPixelMapfv",IoGL_glGetPixelMapfv},
	{"glGetPixelMapuiv",IoGL_glGetPixelMapuiv},
	{"glGetPixelMapusv",IoGL_glGetPixelMapusv},
	{"glGetPolygonStipple",IoGL_glGetPolygonStipple},
	{"glGetString",IoGL_glGetString},
	{"glGetTexEnvfv",IoGL_glGetTexEnvfv},
	{"glGetTexEnviv",IoGL_glGetTexEnvfv}, /*  map to fv function */
	{"glGetTexGendv",IoGL_glGetTexGendv},
	{"glGetTexGenfv",IoGL_glGetTexGendv}, /*  map to dv function */
	{"glGetTexGeniv",IoGL_glGetTexGendv}, /*  map to dv function */
	{"glGetTexImage",IoGL_glGetTexImage},
	{"glGetTexLevelParameterfv",IoGL_glGetTexLevelParameterfv},
	{"glGetTexLevelParameteriv",IoGL_glGetTexLevelParameterfv}, /*  map to fv function */
	{"glGetTexParameterfv",IoGL_glGetTexParameterfv},
	{"glGetTexParameteriv",IoGL_glGetTexParameterfv}, /*  map to fv function */
	{"glHint",IoGL_glHint},
	{"glIndexd",IoGL_glIndexd},
	{"glIndexdv",IoGL_glIndexdv},
	{"glIndexf",IoGL_glIndexd}, /*  map to d function */
	{"glIndexfv",IoGL_glIndexdv}, /*  map to dv function */
	{"glIndexi",IoGL_glIndexd}, /*  map to d function */
	{"glIndexiv",IoGL_glIndexdv}, /*  map to dv function */
	{"glIndexMask",IoGL_glIndexMask},
	{"glIndexPointer",IoGL_glIndexPointer},
	{"glIndexs",IoGL_glIndexd}, /*  map to d function */
	{"glIndexsv",IoGL_glIndexdv}, /*  map to dv function */
	{"glIndexub",IoGL_glIndexd}, /*  map to d function */
	{"glIndexubv",IoGL_glIndexdv}, /*  map to dv function */
	{"glInitNames",IoGL_glInitNames},
	{"glInterleavedArrays",IoGL_glInterleavedArrays},
	{"glIsEnabled",IoGL_glIsEnabled},
	{"glIsList",IoGL_glIsList},
	{"glIsTexture",IoGL_glIsTexture},
	{"glLightf",IoGL_glLightf},
	{"glLightfv",IoGL_glLightfv},
	{"glLighti",IoGL_glLightf}, /*  map to f function */
	{"glLightiv",IoGL_glLightfv}, /*  map to fv function */
	{"glLightModelf",IoGL_glLightModelf},
	{"glLightModeli",IoGL_glLightModelf}, /*  map to f function */
	{"glLineStipple",IoGL_glLineStipple},
	{"glLineWidth",IoGL_glLineWidth},
	{"glListBase",IoGL_glListBase},
	{"glLoadIdentity",IoGL_glLoadIdentity},
	{"glLoadMatrixd",IoGL_glLoadMatrixd},
	{"glLoadMatrixf",IoGL_glLoadMatrixd}, /*  map to d function */
	{"glLoadName",IoGL_glLoadName},
	{"glLogicOp",IoGL_glLogicOp},
	{"glMap1d",IoGL_glMap1d},
	{"glMap1f",IoGL_glMap1d}, /*  map to 1d function */
	{"glMap2d",IoGL_glMap2d},
	{"glMap2f",IoGL_glMap2d}, /*  map to 2d function */
	{"glMapGrid1d",IoGL_glMapGrid1d},
	{"glMapGrid1f",IoGL_glMapGrid1d}, /*  map to 1d function */
	{"glMapGrid2d",IoGL_glMapGrid2d},
	{"glMapGrid2f",IoGL_glMapGrid2d}, /*  map to 2d function */
	{"glMaterialf",IoGL_glMaterialf},
	{"glMaterialfv",IoGL_glMaterialfv},
	{"glMateriali",IoGL_glMaterialf}, /*  map to f function */
	{"glMaterialiv",IoGL_glMaterialfv}, /*  map to fv function */
	{"glMatrixMode",IoGL_glMatrixMode},
	{"glMultMatrixd",IoGL_glMultMatrixd},
	{"glMultMatrixf",IoGL_glMultMatrixd}, /*  map to d function */
	{"glNewList",IoGL_glNewList},
	{"glNormal3b",IoGL_glNormal3d}, /*  map to d function */
	{"glNormal3bv",IoGL_glNormal3dv}, /*  map to dv function */
	{"glNormal3d",IoGL_glNormal3d},
	{"glNormal3dv",IoGL_glNormal3dv},
	{"glNormal3f",IoGL_glNormal3d}, /*  map to d function */
	{"glNormal3fv",IoGL_glNormal3dv}, /*  map to dv function */
	{"glNormal3i",IoGL_glNormal3d}, /*  map to d function */
	{"glNormal3iv",IoGL_glNormal3dv}, /*  map to dv function */
	{"glNormal3s",IoGL_glNormal3d}, /*  map to d function */
	{"glNormal3sv",IoGL_glNormal3dv}, /*  map to dv function */
	{"glNormalPointer",IoGL_glNormalPointer},
	{"glOrtho",IoGL_glOrtho},
	{"glPassThrough",IoGL_glPassThrough},
	{"glPixelMapfv",IoGL_glPixelMapfv},
	{"glPixelMapiv",IoGL_glPixelMapfv}, /*  map to fv function */
	{"glPixelMapsv",IoGL_glPixelMapfv}, /*  map to fv function */
	{"glPixelStoref",IoGL_glPixelStoref},
	{"glPixelStorei",IoGL_glPixelStoref}, /*  map to f function */
	{"glPixelTransferf",IoGL_glPixelTransferf},
	{"glPixelTransferi",IoGL_glPixelTransferi},
	{"glPixelZoom",IoGL_glPixelZoom},
	{"glPointSize",IoGL_glPointSize},
	{"glPolygonMode",IoGL_glPolygonMode},
	{"glPolygonOffset",IoGL_glPolygonOffset},
	{"glPopAttrib",IoGL_glPopAttrib},
	{"glPopClientAttrib",IoGL_glPopClientAttrib},
	{"glPopMatrix",IoGL_glPopMatrix},
	{"glPopName",IoGL_glPopName},
	{"glPrioritizeTextures",IoGL_glPrioritizeTextures},
	{"glPushAttrib",IoGL_glPushAttrib},
	{"glPushClientAttrib",IoGL_glPushClientAttrib},
	{"glPushMatrix",IoGL_glPushMatrix},
	{"glPushName",IoGL_glPushName},
	{"glRasterPos2d",IoGL_glRasterPos2d},
	{"glRasterPos2dv",IoGL_glRasterPos2dv},
	{"glRasterPos2f",IoGL_glRasterPos2d}, /*  map to 2d function */
	{"glRasterPos2fv",IoGL_glRasterPos2dv}, /*  map to 2dv function */
	{"glRasterPos2i",IoGL_glRasterPos2d}, /*  map to 2d function */
	{"glRasterPos2iv",IoGL_glRasterPos2dv}, /*  map to 2dv function */
	{"glRasterPos2s",IoGL_glRasterPos2d}, /*  map to 2d function */
	{"glRasterPos2sv",IoGL_glRasterPos2dv}, /*  map to 2dv function */
	{"glRasterPos3d",IoGL_glRasterPos3d},
	{"glRasterPos3dv",IoGL_glRasterPos3dv},
	{"glRasterPos3f",IoGL_glRasterPos3d}, /*  map to 3d function */
	{"glRasterPos3fv",IoGL_glRasterPos3dv}, /*  map to 3dv function */
	{"glRasterPos3i",IoGL_glRasterPos3d}, /*  map to 3d function */
	{"glRasterPos3iv",IoGL_glRasterPos3dv}, /*  map to 3dv function */
	{"glRasterPos3s",IoGL_glRasterPos3d}, /*  map to 3d function */
	{"glRasterPos3sv",IoGL_glRasterPos3dv}, /*  map to 3dv function */
	{"glRasterPos4d",IoGL_glRasterPos4d},
	{"glRasterPos4dv",IoGL_glRasterPos4dv},
	{"glRasterPos4f",IoGL_glRasterPos4d}, /*  map to 4d function */
	{"glRasterPos4fv",IoGL_glRasterPos4dv}, /*  map to 4dv function */
	{"glRasterPos4i",IoGL_glRasterPos4d}, /*  map to 4d function */
	{"glRasterPos4iv",IoGL_glRasterPos4dv}, /*  map to 4dv function */
	{"glRasterPos4s",IoGL_glRasterPos4d}, /*  map to 4d function */
	{"glRasterPos4sv",IoGL_glRasterPos4dv}, /*  map to 4dv function */
	{"glReadBuffer",IoGL_glReadBuffer},
	{"glReadPixels",IoGL_glReadPixels},
	{"glRectd",IoGL_glRectd},
	{"glRectdv",IoGL_glRectdv},
	{"glRectf",IoGL_glRectd}, /*  map to d function */
	{"glRectfv",IoGL_glRectdv}, /*  map to dv function */
	{"glRecti",IoGL_glRectd}, /*  map to d function */
	{"glRectiv",IoGL_glRectdv}, /*  map to dv function */
	{"glRects",IoGL_glRectd}, /*  map to d function */
	{"glRectsv",IoGL_glRectdv}, /*  map to dv function */
	{"glRenderMode",IoGL_glRenderMode},
	{"glRotated",IoGL_glRotated},
	{"glRotatef",IoGL_glRotated}, /*  map to d function */
	{"glScaled",IoGL_glScaled},
	{"glScalef",IoGL_glScaled}, /*  map to d function */
	{"glScissor",IoGL_glScissor},
	{"glSelectBuffer",IoGL_glSelectBuffer},
	{"glShadeModel",IoGL_glShadeModel},
	{"glStencilFunc",IoGL_glStencilFunc},
	{"glStencilMask",IoGL_glStencilMask},
	{"glStencilOp",IoGL_glStencilOp},
	{"glTexCoord1d",IoGL_glTexCoord1d},
	{"glTexCoord1dv",IoGL_glTexCoord1dv},
	{"glTexCoord1f",IoGL_glTexCoord1d}, /*  map to 1d function */
	{"glTexCoord1fv",IoGL_glTexCoord1dv}, /*  map to 1dv function */
	{"glTexCoord1i",IoGL_glTexCoord1d}, /*  map to 1d function */
	{"glTexCoord1iv",IoGL_glTexCoord1dv}, /*  map to 1dv function */
	{"glTexCoord1s",IoGL_glTexCoord1d}, /*  map to 1d function */
	{"glTexCoord1sv",IoGL_glTexCoord1dv}, /*  map to 1dv function */
	{"glTexCoord2d",IoGL_glTexCoord2d},
	{"glTexCoord2dv",IoGL_glTexCoord2dv},
	{"glTexCoord2f",IoGL_glTexCoord2d}, /*  map to 2d function */
	{"glTexCoord2fv",IoGL_glTexCoord2dv}, /*  map to 2dv function */
	{"glTexCoord2i",IoGL_glTexCoord2d}, /*  map to 2d function */
	{"glTexCoord2iv",IoGL_glTexCoord2dv}, /*  map to 2dv function */
	{"glTexCoord2s",IoGL_glTexCoord2d}, /*  map to 2d function */
	{"glTexCoord2sv",IoGL_glTexCoord2dv}, /*  map to 2dv function */
	{"glTexCoord3d",IoGL_glTexCoord3d},
	{"glTexCoord3dv",IoGL_glTexCoord3dv},
	{"glTexCoord3f",IoGL_glTexCoord3d}, /*  map to 3d function */
	{"glTexCoord3fv",IoGL_glTexCoord3dv}, /*  map to 3dv function */
	{"glTexCoord3i",IoGL_glTexCoord3d}, /*  map to 3d function */
	{"glTexCoord3iv",IoGL_glTexCoord3dv}, /*  map to 3dv function */
	{"glTexCoord3s",IoGL_glTexCoord3d}, /*  map to 3d function */
	{"glTexCoord3sv",IoGL_glTexCoord3dv}, /*  map to 3dv function */
	{"glTexCoord4d",IoGL_glTexCoord4d},
	{"glTexCoord4dv",IoGL_glTexCoord4dv},
	{"glTexCoord4f",IoGL_glTexCoord4d}, /*  map to 4d function */
	{"glTexCoord4fv",IoGL_glTexCoord4dv}, /*  map to 4dv function */
	{"glTexCoord4i",IoGL_glTexCoord4d}, /*  map to 4d function */
	{"glTexCoord4iv",IoGL_glTexCoord4dv}, /*  map to 4dv function */
	{"glTexCoord4s",IoGL_glTexCoord4d}, /*  map to 4d function */
	{"glTexCoord4sv",IoGL_glTexCoord4dv}, /*  map to 4dv function */
	{"glTexCoordPointer",IoGL_glTexCoordPointer},
	{"glTexEnvf",IoGL_glTexEnvf},
	{"glTexEnvfv",IoGL_glTexEnvfv},
	{"glTexEnvi",IoGL_glTexEnvf}, /*  map to f function */
	{"glTexEnviv",IoGL_glTexEnvfv}, /*  map to fv function */
	{"glTexImage1D",IoGL_glTexImage1D},
	{"glTexImage2D",IoGL_glTexImage2D},
	{"glTexParameterf",IoGL_glTexParameterf},
	{"glTexParameterfv",IoGL_glTexParameterfv},
	{"glTexParameteri",IoGL_glTexParameterf}, /*  map to f function */
	{"glTexParameteriv",IoGL_glTexParameterfv}, /*  map to fv function */
	{"glTexSubImage1D",IoGL_glTexSubImage1D},
	{"glTexSubImage2D",IoGL_glTexSubImage2D},
	{"glTranslated",IoGL_glTranslated},
	{"glTranslatei",IoGL_glTranslatei},
	{"glTranslatef",IoGL_glTranslated}, /*  map to d function */
	{"glVertex2d",IoGL_glVertex2d},
	{"glVertex2dv",IoGL_glVertex2dv},
	{"glVertex2f",IoGL_glVertex2d}, /*  map to 2d function */
	{"glVertex2fv",IoGL_glVertex2dv}, /*  map to 2dv function */
	{"glVertex2i",IoGL_glVertex2i}, /*  map to 2d function */
	{"glVertex2iv",IoGL_glVertex2dv}, /*  map to 2dv function */
	{"glVertex2s",IoGL_glVertex2d}, /*  map to 2d function */
	{"glVertex2sv",IoGL_glVertex2dv}, /*  map to 2dv function */
	{"glVertex3d",IoGL_glVertex3d},
	{"glVertex3dv",IoGL_glVertex3dv},
	{"glVertex3f",IoGL_glVertex3d}, /*  map to 3d function */
	{"glVertex3fv",IoGL_glVertex3dv}, /*  map to 3dv function */
	{"glVertex3i",IoGL_glVertex3d}, /*  map to 3d function */
	{"glVertex3iv",IoGL_glVertex3dv}, /*  map to 3dv function */
	{"glVertex3s",IoGL_glVertex3d}, /*  map to 3d function */
	{"glVertex3sv",IoGL_glVertex3dv}, /*  map to 3dv function */
	{"glVertex4d",IoGL_glVertex4d},
	{"glVertex4dv",IoGL_glVertex4dv},
	{"glVertex4f",IoGL_glVertex4d}, /*  map to 4d function */
	{"glVertex4fv",IoGL_glVertex4dv}, /*  map to 4dv function */
	{"glVertex4i",IoGL_glVertex4d}, /*  map to 4d function */
	{"glVertex4iv",IoGL_glVertex4dv}, /*  map to 4dv function */
	{"glVertex4s",IoGL_glVertex4d}, /*  map to 4d function */
	{"glVertex4sv",IoGL_glVertex4dv}, /*  map to 4dv function */
	{"glVertexPointer",IoGL_glVertexPointer},
	{"glViewport",IoGL_glViewport},

	/* The end */
	{NULL,NULL},
};
