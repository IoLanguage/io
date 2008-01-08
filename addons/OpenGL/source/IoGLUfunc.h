/*
 *
 * GLU Functions
 *
 */

typedef struct t_ioGLU_funcTable {
	char *name;
	IoObject* (*func)(IoObject*,IoObject*,IoMessage*);
} t_ioGLU_funcTable;


t_ioGLU_funcTable ioGLU_funcTable[] = {

	/* GLU */
	{"gluLookAt",IoGLU_gluLookAt},
	{"gluPerspective",IoGLU_gluPerspective},
	{"gluOrtho2D",IoGLU_gluOrtho2D},
	{"gluBuild1DMipmaps",IoGLU_gluBuild1DMipmaps},
	{"gluBuild2DMipmaps",IoGLU_gluBuild2DMipmaps},
	{"gluErrorString",IoGLU_gluErrorString},
	{"gluGetString",IoGLU_gluGetString},
	{"gluPickMatrix",IoGLU_gluPickMatrix},
	{"gluProject",IoGLU_gluProject},
	{"gluProjectOrigin",IoGLU_gluProjectOrigin},
	{"gluUnProject",IoGLU_gluUnProject},
	{"gluUnProjectOrigin",IoGLU_gluUnProjectOrigin},
	{"gluScaleImage",IoGLU_gluScaleImage},

	{"gluNewQuadric",IoGLU_gluNewQuadric},
	{"gluDeleteQuadric",IoGLU_gluDeleteQuadric},
	{"gluDisk",IoGLU_gluDisk},
	{"gluPartialDisk",IoGLU_gluPartialDisk},
	{"gluCylinder",IoGLU_gluCylinder},
	{"gluQuadricDrawStyle",IoGLU_gluQuadricDrawStyle},
	{"gluQuadricNormals",IoGLU_gluQuadricNormals},
	{"gluQuadricOrientation",IoGLU_gluQuadricOrientation},
	{"gluQuadricTexture",IoGLU_gluQuadricTexture},
	{"gluSphere",IoGLU_gluSphere},
	{"gluRoundedBox",IoGLU_gluRoundedBox},
	{"gluRoundedBoxOutline",IoGLU_gluRoundedBoxOutline},

	/* The end */
	{NULL,NULL},
};
