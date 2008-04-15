#include <winsock2.h>
#include <windows.h>		
#include <gl\gl.h>			
#include <gl\glu.h>			
#include <gl\glut.h>			
#include "IoState.h"
#include "IoOpenGL.h"
#include "IoGLUT.h"

#include "IoCFunction.h"

#include <tchar.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

HDC			hDC=NULL;		
HGLRC		hRC=NULL;		
HWND		hWnd=NULL;		
HINSTANCE	hInstance;		

BOOL	keys[256];			
BOOL	active=TRUE;		
BOOL	fullscreen=FALSE;	
IoState *ioState;

struct glutOptions {
	BOOL doesReshape;
	BOOL doesDisplay;
	BOOL doesMouse;
	BOOL doesMotion;
	BOOL doesSpecial;
	BOOL doesKeyboard;
	BOOL doesPassiveMotion;
	BOOL doesEntry;
} glutOptions = {0};

struct ioGlutFuncs {
IoObject * (__cdecl *IoGLUT_glutEventTarget_)(IoGLUT *, IoObject *, IoMessage *);
void (__cdecl *IoGlutKeyboardFunc)(unsigned char key, int xv, int yv);
void (__cdecl *IoGlutSpecialFunc)(int key, int xv, int yv);
void (__cdecl *IoGlutKeyboardUpFunc)(unsigned char key, int xv, int yv);
void (__cdecl *IoGlutSpecialUpFunc)(int key, int xv, int yv);
void (__cdecl *IoGlutEntryFunc)(int state);
void (__cdecl *IoGlutMotionFunc)(int xv, int yv);
void (__cdecl *IoGlutPassiveMotionFunc)(int xv, int yv);
void (__cdecl *IoGlutMouseFunc)(int button, int state, int xv, int yv);
void (__cdecl *IoGlutDisplayFunc)(void);
void (__cdecl *IoGlutReshapeFunc)(int width, int height);
void (__cdecl *IoGlutTimerFunc)(int vv);
} ioGlutFuncs;

BOOL loadIoGlutDll()
{
	if (ioGlutFuncs.IoGLUT_glutEventTarget_ == NULL)
	{
		HMODULE hLib = LoadLibrary(_T("\\usr\\local\\lib\\io\\addons\\OpenGL\\_build\\dll\\libioopengl.dll"));
		if (hLib == 0)
			return FALSE;
		ioGlutFuncs.IoGLUT_glutEventTarget_ = (void *)GetProcAddress(hLib, "IoGLUT_glutEventTarget_");
		ioGlutFuncs.IoGlutSpecialFunc = (void *)GetProcAddress(hLib, "IoGlutSpecialFunc");
		ioGlutFuncs.IoGlutKeyboardUpFunc = (void *)GetProcAddress(hLib, "IoGlutKeyboardUpFunc");
		ioGlutFuncs.IoGlutEntryFunc = (void *)GetProcAddress(hLib, "IoGlutEntryFunc");
		ioGlutFuncs.IoGlutMotionFunc = (void *)GetProcAddress(hLib, "IoGlutMotionFunc");
		ioGlutFuncs.IoGlutPassiveMotionFunc = (void *)GetProcAddress(hLib, "IoGlutPassiveMotionFunc");
		ioGlutFuncs.IoGlutMouseFunc = (void *)GetProcAddress(hLib, "IoGlutMouseFunc");
		ioGlutFuncs.IoGlutDisplayFunc = (void *)GetProcAddress(hLib, "IoGlutDisplayFunc");
		ioGlutFuncs.IoGlutReshapeFunc = (void *)GetProcAddress(hLib, "IoGlutReshapeFunc");
		ioGlutFuncs.IoGlutTimerFunc = (void *)GetProcAddress(hLib, "IoGlutTimerFunc");
		return TRUE;
	} else
		return TRUE;
}
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	

void ReSizeGLScene(GLsizei width, GLsizei height)		
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

}

int InitGL()										
{
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	return TRUE;										
}

int DrawGLScene()									
{
	if (glutOptions.doesDisplay)
		if (loadIoGlutDll()) ioGlutFuncs.IoGlutDisplayFunc();
	/*
	static float t = 0;
	t+=0.1f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glRotatef(t, 0, 0, 1);
	glScalef(0.1f, 0.1f, 0.1f);

	glBegin(GL_QUADS);
	glColor3f(1, 0, 0.5f);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
	*/
	return TRUE;										
}

void KillGLWindow()								
{
	if (fullscreen)										
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);								
	}

	if (hRC)											
	{
		if (!wglMakeCurrent(NULL,NULL))					
		{
			MessageBox(NULL,_T("Release Of DC And RC Failed."),_T("SHUTDOWN ERROR"),MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						
		{
			MessageBox(NULL,_T("Release Rendering Context Failed."),_T("SHUTDOWN ERROR"),MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					
	{
		MessageBox(NULL,_T("Release Device Context Failed."),_T("SHUTDOWN ERROR"),MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										
	}

	if (hWnd && !DestroyWindow(hWnd))					
	{
		MessageBox(NULL,_T("Could Not Release hWnd."),_T("SHUTDOWN ERROR"),MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										
	}

	if (!UnregisterClass(_T("OpenGL"),hInstance))			
	{
		MessageBox(NULL,_T("Could Not Unregister Class."),_T("SHUTDOWN ERROR"),MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									
	}
}

/* convert VK_* codes to glut special key constants. Return -1 if no match */
int GetGlutSpecialKeyCode(WPARAM vkKeyCode)
{
	switch (vkKeyCode)
	{
	case VK_F1:
		return GLUT_KEY_F1;
	case VK_F2:
		return GLUT_KEY_F2;
	case VK_F3:
		return GLUT_KEY_F3;
	case VK_F4:
		return GLUT_KEY_F4;
	case VK_F5:
		return GLUT_KEY_F5;
	case VK_F6:
		return GLUT_KEY_F6;
	case VK_F7:
		return GLUT_KEY_F7;
	case VK_F8:
		return GLUT_KEY_F8;
	case VK_F9:
		return GLUT_KEY_F9;
	case VK_F10:
		return GLUT_KEY_F10;
	case VK_F11:
		return GLUT_KEY_F11;
	case VK_F12:
		return GLUT_KEY_F12;
	case VK_LEFT:
		return GLUT_KEY_LEFT;
	case VK_RIGHT:
		return GLUT_KEY_RIGHT;
	case VK_UP:
		return GLUT_KEY_UP;
	case VK_DOWN:
		return GLUT_KEY_DOWN;
	case VK_PRIOR:
		return GLUT_KEY_PAGE_UP;
	case VK_NEXT:
		return GLUT_KEY_PAGE_DOWN;
	case VK_HOME:
		return GLUT_KEY_HOME;
	case VK_END:
		return GLUT_KEY_END;
	case VK_INSERT:
		return GLUT_KEY_INSERT;

	default:
		return -1;
	}
}
/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag)
{
	GLuint		PixelFormat;			
	WNDCLASS	wc;						
	DWORD		dwExStyle;				
	DWORD		dwStyle;				

	static	PIXELFORMATDESCRIPTOR pfd=			
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		0,						    				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
    };

	RECT		WindowRect;				
	WindowRect.left=(long)0;			
	WindowRect.right=(long)width;		
	WindowRect.top=(long)0;				
	WindowRect.bottom=(long)height;		

	fullscreen=fullscreenflag;			

	hInstance			= GetModuleHandle(NULL);				
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	wc.lpfnWndProc		= (WNDPROC) WndProc;					
	wc.cbClsExtra		= 0;									
	wc.cbWndExtra		= 0;									
	wc.hInstance		= hInstance;							
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			
	wc.hbrBackground	= NULL;									
	wc.lpszMenuName		= NULL;									
	wc.lpszClassName	= _T("OpenGL");							

	if (!RegisterClass(&wc))									
	{
		MessageBox(NULL,_T("Failed To Register The Window Class."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}
	
	if (fullscreen)												
	{
		DEVMODE dmScreenSettings;								
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,_T("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?"),_T("NeHe GL"),MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		
			}
			else
				return FALSE;									
		}
	}

	if (fullscreen)												
	{
		dwExStyle=WS_EX_APPWINDOW;								
		dwStyle=WS_POPUP;										
		ShowCursor(FALSE);										
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			
		dwStyle=WS_OVERLAPPEDWINDOW;							
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	

	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								_T("OpenGL"),						// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								CW_USEDEFAULT, CW_USEDEFAULT,		// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Window Creation Error."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}


    pfd.cColorBits = bits;
    
    if (!(hDC=GetDC(hWnd)))                         
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Can't Create A GL Device Context."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Can't Find A Suitable PixelFormat."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Can't Set The PixelFormat."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Can't Create A GL Rendering Context."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!wglMakeCurrent(hDC,hRC))					
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Can't Activate The GL Rendering Context."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);						
	SetForegroundWindow(hWnd);						
	SetFocus(hWnd);									
	ReSizeGLScene(width, height);					

	if (!InitGL())									
	{
		KillGLWindow();								
		MessageBox(NULL,_T("Initialization Failed."),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;									
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			
							UINT	uMsg,			
							WPARAM	wParam,			
							LPARAM	lParam)			
{
	switch (uMsg)									
	{
		case WM_ACTIVATE:							
		{
			if (!HIWORD(wParam))					
			{
				active=TRUE;						
			}
			else
			{
				active=FALSE;						
			}
			return 0;								
		}

		case WM_SYSCOMMAND:							
		{
			switch (wParam)							
			{
				case SC_SCREENSAVE:					
				case SC_MONITORPOWER:				
				return 0;							// Prevent From Happening
			}
			break;									
		}
		case WM_MOUSEMOVE:
		{			
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
				if (glutOptions.doesMotion)
					if (loadIoGlutDll()) ioGlutFuncs.IoGlutMotionFunc(x, y);
			else
				if (glutOptions.doesPassiveMotion)
					if (loadIoGlutDll()) ioGlutFuncs.IoGlutPassiveMotionFunc(x, y);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			
			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_RIGHT_BUTTON, GLUT_DOWN, x, y);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_MIDDLE_BUTTON, GLUT_DOWN, x, y);
			break;
		}
		case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_LEFT_BUTTON, GLUT_UP, x, y);
			break;
		}
		case WM_RBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_RIGHT_BUTTON, GLUT_UP, x, y);
			break;
		}
		case WM_MBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			if (glutOptions.doesMouse)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutMouseFunc(GLUT_MIDDLE_BUTTON, GLUT_UP, x, y);
			break;
		}
		case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;								
		}

		case WM_KEYDOWN:							
		{
			POINT ptCursor;
			int glutKey = GetGlutSpecialKeyCode(wParam);
			GetCursorPos(&ptCursor);
			ScreenToClient(hWnd, &ptCursor);

			keys[wParam] = TRUE;					
			if (glutOptions.doesSpecial && glutKey != -1)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutSpecialFunc(glutKey, ptCursor.x, ptCursor.y);
			
			return 0;								
		}

		case WM_KEYUP:								
		{
			keys[wParam] = FALSE;					
			return 0;								
		}

		case WM_CHAR:
		{
			char c = wParam;
			POINT ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(hWnd, &ptCursor);

			if (glutOptions.doesKeyboard)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutKeyboardFunc(c, ptCursor.x, ptCursor.y);

			break;
		}	

		case WM_SIZE:								
		{
			if (glutOptions.doesReshape)
				if (loadIoGlutDll()) ioGlutFuncs.IoGlutReshapeFunc(LOWORD(lParam),HIWORD(lParam));
			return 0;								
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void StartIO(LPCTSTR aScriptName)
{
	ioState = IoState_new();	
	IoState_init(ioState);
	
//	IoState_argc_argv_(ioState, 1, pbuf);
	IoState_runCLI(ioState);
//	IoState_doFile_(ioState, aScriptName);
}

void StopIO()
{
	if (ioState) IoState_free(ioState);
}

void GLIoView_postRedisplay(IoGL *self, IoObject *locals, IoMessage *m)
{
//	glutOptions.doesDisplay = TRUE; !
}

void GLIoView_glutReshapeFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesReshape = TRUE;
}

void GLIoView_glutDisplayFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesDisplay = TRUE;
}

void GLIoView_glutEntryFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesEntry = TRUE;
}

void GLIoView_glutMouseFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesMouse = TRUE;
}

void GLIoView_glutMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesMotion = TRUE;
}

void GLIoView_glutSpecialFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesSpecial = TRUE;
}

void GLIoView_glutKeyboardFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesKeyboard = TRUE;
}

void GLIoView_glutPassiveMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
	glutOptions.doesPassiveMotion = TRUE;
}

int g_timerVal;

void CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (loadIoGlutDll()) ioGlutFuncs.IoGlutTimerFunc(g_timerVal);
}

void GLIoView_registerTimer(IoGL *self, IoObject *locals, IoMessage *m)
{
	int millisecs = IoMessage_locals_intArgAt_(m, locals, 0);
	int val = IoMessage_locals_intArgAt_(m, locals, 1);

	g_timerVal = val;
	SetTimer(NULL, 1, millisecs, timerProc);
}

void GLIoView_noop(IoGL *self, IoObject *locals, IoMessage *m) 
{
}

#define IOCFUNCTION_GL(func) IoCFunction_newWithFunctionPointer_tag_name_(ioState, (void *)func, NULL, "")

void overrideIoGLMethods()
{
	IoObject *cxt = IoState_doCString_(ioState, "GLUT");

	IoCFunction *noopfunc = IOCFUNCTION_GL(GLIoView_noop);
	
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutDisplayFunc"),
						 IOCFUNCTION_GL(GLIoView_glutDisplayFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutReshapeFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutReshapeFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutSpecialFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutSpecialFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutKeyboardFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutKeyboardUpFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMouseFunc"),
						 IOCFUNCTION_GL(GLIoView_glutMouseFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMotionFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutMotionFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutPassiveMotionFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutPassiveMotionFunc));
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutEntryFunc"), 
						 IOCFUNCTION_GL(GLIoView_glutEntryFunc));
    
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutVisibilityFunc"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutTimerFunc"), 
						 IOCFUNCTION_GL(GLIoView_registerTimer));
    
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutIdleFunc"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutPostRedisplay"), 
						 IOCFUNCTION_GL(GLIoView_postRedisplay));
	
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInit"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMainLoop"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitWindowSize"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutCreateWindow"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitWindowPosition"), noopfunc);
	/*
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutEventTarget"), 
						 IOCFUNCTION_GL(IoGLUT_glutEventTarget_));  
    */
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutSwapBuffers"), 
						 IOCFUNCTION_GL(GLIoView_postRedisplay));
}

int main(int argc, char **argv)
{
	ioState = IoState_new();	
	IoState_init(ioState);
	overrideIoGLMethods();
//	IoState_doFile_(ioState, "main.io");
	IoState_argc_argv_(ioState, argc, argv);
	IoState_runCLI(ioState);
	StartGL();
	if (ioState) IoState_free(ioState);
}

int StartGL()
{
	MSG		msg;									
	BOOL	done=FALSE;								

//	StartIO(_T("main.io"));
//	IoState_doFile_(ioState, "main.io");

	// Create Our OpenGL Window
	if (!CreateGLWindow(_T("NeHe's OpenGL Framework"),640,480,32,fullscreen))
	{
		return 0;									
	}

	if (glutOptions.doesReshape)
		if (loadIoGlutDll()) ioGlutFuncs.IoGlutReshapeFunc(640,480);

	while(!done)									
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if (msg.message==WM_QUIT)				
			{
				done=TRUE;							
			}
			else									
			{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);				
			}
		}
		else										
		{
			if (active)								
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done=TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					DrawGLScene();					
					SwapBuffers(hDC);				
				}
			}

			if (keys[VK_RETURN])					
			{
				keys[VK_RETURN]=FALSE;				
				KillGLWindow();						
				fullscreen=!fullscreen;				
				// Recreate Our OpenGL Window
				if (!CreateGLWindow(_T("NeHe's OpenGL Framework"),640,480,32,fullscreen))
				{
					return 0;						
				}
			}
		}
	}

	// Shutdown
	StopIO();
	KillGLWindow();									
	return (msg.wParam);							
}

int WINAPI WinMain(	HINSTANCE	hInstance,			
					HINSTANCE	hPrevInstance,		
					LPSTR		lpCmdLine,			
					int			nCmdShow)			
{
	return StartGL();
}
