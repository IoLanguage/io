/*
 *
 * GLUT Functions
 *
 */

typedef struct t_ioGLUT_funcTable {
	char		*name;
	IoObject*	(*func)(IoObject*,IoObject*,IoMessage*);
} t_ioGLUT_funcTable;


t_ioGLUT_funcTable ioGLUT_funcTable[] = {

	{"glutCreateSubWindow", IoGLUT_glutCreateSubWindow},
	{"glutCreateWindow", IoGLUT_glutCreateWindow},
	{"glutDestroyWindow", IoGLUT_glutDestroyWindow},
	{"glutDisplayFunc", IoGLUT_glutDisplayFunc},
	{"glutEnterGameMode", IoGLUT_glutEnterGameMode},
	{"glutLeaveGameMode", IoGLUT_glutLeaveGameMode},
	{"glutEntryFunc", IoGLUT_glutEntryFunc},
	{"glutEventTarget", IoGLUT_glutEventTarget_},
	{"glutGameModeString", IoGLUT_glutGameModeString},
	{"glutGetWindow", IoGLUT_glutGetWindow},
	{"glutGet", IoGLUT_glutGet},
	{"glutHideWindow", IoGLUT_glutHideWindow},
	{"glutIconifyWindow", IoGLUT_glutIconifyWindow},
	{"glutIgnoreKeyRepeat", IoGLUT_glutIgnoreKeyRepeat},
	{"glutInitDisplayMode", IoGLUT_glutInitDisplayMode},
	{"glutInit", IoGLUT_glutInit},
	{"glutInitWindowPosition", IoGLUT_glutInitWindowPosition},
	{"glutInitWindowSize", IoGLUT_glutInitWindowSize},
	{"glutKeyboardFunc", IoGLUT_glutKeyboardFunc},
	{"glutKeyboardUpFunc", IoGLUT_glutKeyboardUpFunc},
	{"glutMainLoop", IoGLUT_glutMainLoop},
	{"glutGetModifiers", IoGLUT_glutGetModifiers},
	{"glutMotionFunc", IoGLUT_glutMotionFunc},
	{"glutMouseFunc", IoGLUT_glutMouseFunc},
	{"glutPassiveMotionFunc", IoGLUT_glutPassiveMotionFunc},
	{"glutPopWindow", IoGLUT_glutPopWindow},
	{"glutPositionWindow", IoGLUT_glutPositionWindow},
	{"glutPostRedisplay", IoGLUT_glutPostRedisplay},
	{"glutPushWindow", IoGLUT_glutPushWindow},
	{"glutReshapeFunc", IoGLUT_glutReshapeFunc},
	{"glutReshapeWindow", IoGLUT_glutReshapeWindow},
	{"glutSetCursor", IoGLUT_glutSetCursor},
	{"glutSetIconTitle", IoGLUT_glutSetIconTitle},
	{"glutSetWindow", IoGLUT_glutSetWindow},
	{"glutSetWindowTitle", IoGLUT_glutSetWindowTitle},
	{"glutShowWindow", IoGLUT_glutShowWindow},
	{"glutSolidCone", IoGLUT_glutSolidCone},
	{"glutSolidCube", IoGLUT_glutSolidCube},
	{"glutSolidSphere", IoGLUT_glutSolidSphere},
	{"glutSolidTorus", IoGLUT_glutSolidTorus},
	{"glutSpecialFunc", IoGLUT_glutSpecialFunc},
	{"glutSpecialUpFunc", IoGLUT_glutSpecialUpFunc},
	{"glutStrokeCharacter", IoGLUT_glutStrokeCharacter},
	{"glutStrokeString", IoGLUT_glutStrokeString},
	{"glutSwapBuffers", IoGLUT_glutSwapBuffers},
	{"glutTimerFunc", IoGLUT_glutTimerFunc},
	{"glutWarpPointer", IoGLUT_glutWarpPointer},
	{"glutWireCone", IoGLUT_glutWireCone},
	{"glutWireCube", IoGLUT_glutWireCube},
	{"glutWireSphere", IoGLUT_glutWireSphere},
	{"glutWireTorus", IoGLUT_glutWireTorus},

	{"glutSolidDodecahedron", IoGLUT_glutSolidDodecahedron},
	{"glutWireDodecahedron", IoGLUT_glutWireDodecahedron},

	{"glutSolidOctahedron", IoGLUT_glutSolidOctahedron},
	{"glutWireOctahedron", IoGLUT_glutWireOctahedron},

	{"glutSolidTetrahedron", IoGLUT_glutSolidTetrahedron},
	{"glutWireTetrahedron", IoGLUT_glutWireTetrahedron},

	{"glutSolidIcosahedron", IoGLUT_glutSolidIcosahedron},
	{"glutWireIcosahedron", IoGLUT_glutWireIcosahedron},

	{"glutSolidTeapot", IoGLUT_glutSolidTeapot},
	{"glutWireTeapot", IoGLUT_glutWireTeapot},
	{"glutFullScreen", IoGLUT_glutFullScreen},


	{"glutCreateMenu", IoGLUT_glutCreateMenu},
	{"glutSetMenu", IoGLUT_glutSetMenu},
	{"glutGetMenu", IoGLUT_glutGetMenu},
	{"glutDestroyMenu", IoGLUT_glutDestroyMenu},
	{"glutAddMenuEntry", IoGLUT_glutAddMenuEntry},
	{"glutAddSubMenu", IoGLUT_glutAddSubMenu},

	{"glutChangeToMenuEntry", IoGLUT_glutChangeToMenuEntry},
	{"glutChangeToSubMenu", IoGLUT_glutChangeToSubMenu},
	{"glutRemoveMenuItem", IoGLUT_glutRemoveMenuItem},
	{"glutAttachMenu", IoGLUT_glutAttachMenu},
	{"glutDetachMenu", IoGLUT_glutDetachMenu},

	//{"glutInitJoystick", IoGLUT_glutInitJoystick},
	{"glutJoystickFunc", IoGLUT_glutJoystickFunc},

	/* The end */
	{NULL,NULL},
};
