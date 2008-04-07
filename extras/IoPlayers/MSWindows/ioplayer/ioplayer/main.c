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

		case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;								
		}

		case WM_KEYDOWN:							
		{
			keys[wParam] = TRUE;					
			return 0;								
		}

		case WM_KEYUP:								
		{
			keys[wParam] = FALSE;					
			return 0;								
		}

		case WM_SIZE:								
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			return 0;								
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void CreateConsole()
{
	int hConHandle;
	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// allocate a console for this app
	AllocConsole();

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

}

	char buf[100] = {"boo"};
	char *pbuf[] = {&buf};

void StartIO(LPCTSTR aScriptName)
{
	CreateConsole();
	ioState = IoState_new();	
	IoState_init(ioState);
	
	IoState_argc_argv_(ioState, 1, pbuf);
	IoState_runCLI(ioState);
//	IoState_doFile_(ioState, aScriptName);
}

void StopIO()
{
	if (ioState) IoState_free(ioState);
	FreeConsole();
}

void GLIoView_postRedisplay(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutReshapeFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutDisplayFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutEntryFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutMouseFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutKeyboardFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_glutPassiveMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
}

void GLIoView_registerTimer(IoGL *self, IoObject *locals, IoMessage *m)
{
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
	
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutEventTarget"), 
						 IOCFUNCTION_GL(IoGLUT_glutEventTarget_));  
    
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
	IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutSwapBuffers"), 
						 IOCFUNCTION_GL(GLIoView_postRedisplay));
}

int main(int argc, char **argv)
{
	ioState = IoState_new();	
	IoState_init(ioState);
	
//	IoState_argc_argv_(ioState, argc, argv);
//	IoState_runCLI(ioState);
//	IoState_doFile_(ioState, "main.io");
	StartGL();
	if (ioState) IoState_free(ioState);
}

int StartGL()
{
	MSG		msg;									
	BOOL	done=FALSE;								

	StartIO(_T("main.io"));

	// Create Our OpenGL Window
	if (!CreateGLWindow(_T("NeHe's OpenGL Framework"),640,480,32,fullscreen))
	{
		return 0;									
	}


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
