//========================================================================
//	file:		r_gl2_driver.cpp
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_gl2_common.h"

// engine headers.
#include "../engine/e_system.h"
#include "../engine/gr_driver.h"

// glut headers.
#include "../../lib/freeglut-2.6.0/include/GL/freeglut.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GL2Driver;
GL2Driver*					gDriver;

extern bool					GL2_Startup(const wstring& windowTitle);
extern bool					GL2_Shutdown();
extern bool					GL2_SwapBuffers();
//========================================================================

//========================================================================
// GL2Driver
//========================================================================
class GL2Driver : public GrDriver
{
private:
	bool		_fatalError;
	int			_winIdx;

	//===================
	// GlutOnDisplay
	//===================
	static void		GlutOnDisplay()
	{
		GL2Driver* driver(gDriver);
		E_VERIFY(driver != NULL, return);

		// clear the screen and the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// reset the current modelview matrix
		glLoadIdentity();

		// draw "hello world" primitives.
		glTranslatef(-1.5f, 0.0f, -6.0f);
		glBegin(GL_TRIANGLES);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);
		glEnd();
		glTranslatef(3.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);
		glEnd();

		// display the backbuffer to the window.
		if (!GL2_SwapBuffers())
			driver->_fatalError = true;
	}


	//===================
	// GlutOnWindowResized
	//===================
	static void		GlutOnWindowResized(int width, int height)
	{
		height = E_MAX(1, height);
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// calculate the aspect ratio of the window
		const float fovy(60.0f);
		const float aspect(width / (float)height);
		const float znear(1.0f);
		const float zfar(100.0f);
		gluPerspective(fovy, aspect, znear, zfar);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}


public:
	//===================
	// GL2::GL2
	//===================
	GL2Driver(int windowWidth, int windowHeight, const wstring& windowTitle)
		: _fatalError(false)
		, _winIdx(0)
	{
		// validate parameters.
		windowWidth = E_MAX(640, windowWidth);
		windowHeight = E_MAX(480, windowWidth);

		// get the exe path.
		string exePath(WStringToString(gSystem->GetExePath()));
		char exePathBuf[1024];
		MemCpy(exePathBuf, exePath.c_str(), exePath.size()+1);

		// intialize glut.
		int argc(1);
		char* argv(exePathBuf);
		glutInit(&argc, &argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(windowWidth, windowHeight);
		glutInitWindowPosition(100, 100);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

		// create the window.
		_winIdx = glutCreateWindow(WStringToString(windowTitle).c_str());
		if (_winIdx != 0)
		{
			// initialize GL2
			glShadeModel(GL_SMOOTH);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			// set glut callbacks.
			glutDisplayFunc(&GlutOnDisplay);
			glutReshapeFunc(&GlutOnWindowResized);

			if (!GL2_Startup(windowTitle))
			{
				E_WARN("gl2", "GL2_Startup failed");
				glutDestroyWindow(_winIdx);
				_winIdx = 0;
			}
		}
	}


	//===================
	// GL2::~GL2
	//===================
	~GL2Driver()
	{
		GL2_Shutdown();
	}


	//===================
	// GL2::HasFatalError
	//===================
	bool		HasFatalError()
	{
		return _fatalError;
	}

	//===================
	// GL2::BeginFrame
	//===================
	bool		BeginFrame()
	{
		if (_fatalError)
			return false;

		// if the user has closed the window, abort.
		if (glutGetWindow() == 0)
			return false;

		glutMainLoopEvent();

		return true;
	}

	//===================
	// GL2::Render
	//===================
	void		Render()
	{
		E_VERIFY(!_fatalError, return);
	}

	//===================
	// GL2::EndFrame
	//===================
	void		EndFrame()
	{
		E_VERIFY(!_fatalError, return);
	}
};

//===================
// RendererStartup
//===================
RENDER_GL2_EXPORT void*			RendererStartup(int version,
												int windowWidth, int windowHeight,
												const wstring& windowTitle)
{
	E_VERIFY(version == GR_LIB_VERSION, return NULL);

	// if we're already initialized, use the current driver.
	assert(gDriver == NULL);
	if (gDriver != NULL)
		return gDriver;

	if (windowTitle.empty())
	{
		gSystem->Warn(_T("gl2"), _T("Failed to initialize GL2 renderer: must specify a unique window title."));
		return NULL;
	}

	// startup the video driver and return a pointer to it.
	gDriver = E_NEW("gl2", GL2Driver)(windowWidth, windowHeight, windowTitle);
	return gDriver;
}

//===================
// RendererShutdown
//===================
RENDER_GL2_EXPORT bool			RendererShutdown(void* driverPtr)
{
	E_VERIFY(gDriver != NULL, return false);

	// verify that the caller is passing in the same memory we returned to it.
	E_VERIFY(driverPtr == gDriver, return false);

	// shutdown the video driver.
	GrDriver* driver((GrDriver*)driverPtr);
	E_DELETE("gl2", driver);
	gDriver = NULL;

	return true;
}

