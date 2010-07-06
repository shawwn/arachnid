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
#include "r_gl2_driver.h"

// engine headers.
#include "../engine/e_system.h"

// graphics headers.
#include "../engine/gr_driver.h"
#include "../engine/gr_scene.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_material.h"

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

//===================
// GL2Driver::GlutOnDisplay
//===================
void		GL2Driver::GlutOnDisplay()
{
	GL2Driver* driver(gDriver);
	E_VERIFY(driver != NULL, return);

	// clear the screen and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset the current modelview matrix
	glLoadIdentity();

	// render the scene.
	driver->RenderModel(driver->GetScene().GetModel());

	// display the backbuffer to the window.
	if (!GL2_SwapBuffers())
		driver->_fatalError = true;
}


//===================
// GL2Driver::GlutOnWindowResized
//===================
void		GL2Driver::GlutOnWindowResized(int width, int height)
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


//===================
// GL2Driver::GL2Driver
//===================
GL2Driver::GL2Driver(int windowWidth, int windowHeight, const wstring& windowTitle)
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
// GL2Driver::RenderModelNode
//===================
void		GL2Driver::RenderModelNode(const GrModelNode& node)
{
	const MMat44& transform(node.GetTransform());

	GrMesh* mesh(node.GetMesh());
	if (mesh != NULL)
	{
		glBegin(GL_TRIANGLES);
		SVec3* positions(mesh->GetPositions());
		TriIdx* indices(mesh->GetTriIndices());
		for (uint tri = 0; tri < mesh->GetNumTriangles(); ++tri)
		{
			for (uint corner = 0; corner < 3; ++corner)
			{
				uint idx(indices[3*tri + corner]);
				SVec3 pos(positions[idx]);
				pos.RotateTranslate(transform);
				glVertex3f(pos.X(), pos.Y(), pos.Z());
			}
		}
		glEnd();
	}

	// render each child.
	for (uint i = 0; i < node.NumChildModelNodes(); ++i)
	{
		const GrModelNode& child(*node.GetChildModelNode(i));
		RenderModelNode(child);
	}
}


//===================
// GL2Driver::RenderModel
//===================
void		GL2Driver::RenderModel(const GrModel& model)
{
	// render the nodes.
	RenderModelNode(model.GetRoot());

	// render each child.
	for (uint i = 0; i < model.NumChildModels(); ++i)
	{
		const GrModel& child(*model.GetChildModel(i));
		RenderModel(child);
	}
}


//===================
// GL2Driver::~GL2Driver
//===================
GL2Driver::~GL2Driver()
{
	GL2_Shutdown();
}


//===================
// GL2Driver::BeginFrame
//===================
bool		GL2Driver::BeginFrame()
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
// GL2Driver::Render
//===================
void		GL2Driver::Render(const GrScene& scene, const GrCamera& camera)
{
	E_VERIFY(!_fatalError, return);
}


//===================
// GL2Driver::EndFrame
//===================
void		GL2Driver::EndFrame()
{
	E_VERIFY(!_fatalError, return);
}


//===================
// GL2Driver::CreateMesh
//===================
GrMesh*		GL2Driver::CreateMesh(
					   const SVec3* positions, const SVec2* texcoords, uint numVerts,
					   const TriIdx* triangles, uint numTris)
{
	// verify input.
	E_VERIFY(numVerts != 0 && numTris != 0, return NULL);
	E_VERIFY(positions != NULL && triangles != NULL, return NULL);

	// prepare the result.
	GrMesh* result(E_NEW("gl2", GrMesh)(this));

	// store the vertex data.
	result->_positions = ArrayCpy(_T("gl2"), positions, numVerts);
	result->_texcoords = ArrayCpy(_T("gl2"), texcoords, numVerts);
	result->_numVertices = numVerts;

	// store the index data.
	result->_triIndices = ArrayCpy(_T("gl2"), triangles, 3*numTris);
	result->_numTriangles = numTris;

	// return the result.
	return result;
}
//========================================================================


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

