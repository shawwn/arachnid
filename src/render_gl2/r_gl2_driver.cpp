//========================================================================
//	file:		r_gl2_driver.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_gl2_common.h"
#include "r_gl2_driver.h"

// engine headers.
#include "../engine/e_engine.h"
#include "../engine/e_system.h"

// graphics headers.
#include "../engine/gr_driver.h"
#include "../engine/gr_scene.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_material.h"
#include "../engine/gr_texture.h"

// glut headers.
#include "../../lib/freeglut-2.6.0/include/GL/freeglut.h"

// GL headers.
#include "glext.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GL2Driver;
GL2Driver*					gDriver;

extern bool					GL2_Startup(const wstring& windowTitle);
extern bool					GL2_Shutdown();
extern bool					GL2_SwapBuffers();
extern bool					GL2_RefreshWindow();
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

	GrCamera* cam(driver->GetCamera());
	if (cam != NULL)
	{
		// apply the camera.
		driver->ApplyCamera(*cam);

		// render the scene.
		driver->RenderModel(driver->GetScene().GetModel());
	}

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
	const float zfar(10000.0f);
	gluPerspective(fovy, aspect, znear, zfar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//===================
// GL2Driver::GlutOnMousePos
//===================
void		GL2Driver::GlutOnMousePos(int x, int y)
{
	gEngine->OnMousePos(x, y);
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
	windowHeight = E_MAX(480, windowHeight);

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
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glDisable(GL_CULL_FACE);

		// set glut callbacks.
		glutDisplayFunc(&GlutOnDisplay);
		glutReshapeFunc(&GlutOnWindowResized);
		glutMotionFunc(&GlutOnMousePos);
		glutPassiveMotionFunc(&GlutOnMousePos);

		if (!GL2_Startup(windowTitle))
		{
			E_WARN("gl2", "GL2_Startup failed");
			glutDestroyWindow(_winIdx);
			_winIdx = 0;
		}
	}

	if (_winIdx != 0)
		_isActive = true;

}


//===================
// GL2Driver::ApplyCamera
//===================
void		GL2Driver::ApplyCamera(const GrCamera& cam)
{
	MVec3 pos, lookAt, up;
	cam.GetEyeInfo(pos, lookAt, up);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		pos.X(), pos.Y(), pos.Z(),
		lookAt.X(), lookAt.Y(), lookAt.Z(),
		up.X(), up.Y(), up.Z());
}


//===================
// GL2Driver::RenderModelNode
//===================
void		GL2Driver::RenderModelNode(GrModelNode& node)
{
	const MTransform& transform(node.GetWorld());

	GrMesh* mesh(node.GetMesh());
	if (mesh != NULL)
	{
		GrSkin* skin(mesh->GetSkin());
		const byte* boneIndices(NULL);
		const float* boneWeights(NULL);
		const MTransform** skeleton = NULL;
		const SVec3* positions(skin->GetPositions());
		const SVec2* texcoords(skin->GetTexcoords());
		const TriIdx* indices(skin->GetIndices());
		if (skin->GetMeshChannels() & MESH_BONE_INFO)
		{
			skeleton = node.GetSkeleton();
			positions = skin->DeformVerts(skeleton);
		}

		E_ASSERT(node.NumMeshRanges() > 0);
		for (uint rangeIdx = 0; rangeIdx < node.NumMeshRanges(); ++rangeIdx)
		{
			GrModelNode::SMeshRange* range(node.GetMeshRange(rangeIdx));
			E_VERIFY(range != NULL, continue);

			// set texture.
			GrTexture* diffuse(range->material->GetTexture(MT_DIFFUSE));
			if (diffuse != NULL)
				glBindTexture(0, (GLuint)(size_t)diffuse->_userdata);
			else
				glBindTexture(0, (GLuint)-1);

			// draw each triangle.
			glBegin(GL_TRIANGLES);
			for (uint tri = range->triStart; tri < range->triStart + range->triCount; ++tri)
			{
				for (uint corner = 0; corner < 3; ++corner)
				{
					uint idx(indices[3*tri + corner]);
					SVec2 uv(texcoords[idx]);
					SVec3 pos(positions[idx]);
					uv.Y() = 1.0f - uv.Y();

					pos.RotateTranslate(transform);
					glTexCoord2f(uv.X(), uv.Y());
					glVertex3f(pos.X(), pos.Y(), pos.Z());
				}
			}
			glEnd();
		}
	}

	// render each child.
	for (uint i = 0; i < node.NumChildModelNodes(); ++i)
	{
		GrModelNode& child(*node.GetChildModelNode(i));
		RenderModelNode(child);
	}
}


//===================
// GL2Driver::RenderModel
//===================
void		GL2Driver::RenderModel(GrModel& model)
{
	// render the nodes.
	RenderModelNode(model.GetRoot());

	// render each child.
	for (uint i = 0; i < model.NumChildModels(); ++i)
	{
		GrModel& child(*model.GetChildModel(i));
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

	// refresh the window.
	if (!GL2_RefreshWindow())
		return false;

	// if the user has closed the window, abort.
	if (glutGetWindow() == 0)
		return false;

	glutMainLoopEvent();

	return true;
}


//===================
// GL2Driver::EndFrame
//===================
void		GL2Driver::EndFrame()
{
	E_VERIFY(!_fatalError, return);
}


//===================
// GL2Driver::SetMousePos
//===================
void		GL2Driver::SetMousePos(int x, int y)
{
	glutWarpPointer(x, y);
}


//===================
// GL2Driver::CreateMesh
//===================
GrMesh*		GL2Driver::CreateMesh(const wchar_t* ctx, GrSkin* geometry)
{
	// verify input.
	E_VERIFY(geometry->GetNumVerts() != 0 && geometry->GetNumTris() != 0, return NULL);

	// prepare the result.
	GrMesh* result(E_NEW("gl2", GrMesh)(this));

	// store the geometry.
	result->_skin = geometry;

	// return the result.
	return result;
}


//===================
// GL2Driver::CreateTexture
//===================
GrTexture*	GL2Driver::CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height)
{
	E_VERIFY(!_fatalError, return NULL);

	// verify input.
	E_VERIFY(bgra != NULL, return NULL);
	E_VERIFY(width > 0 && height > 0, return NULL);

	// create the GL2 texture.
	GLuint id(-1);
	glGenTextures(1, &id);

	// fill the texture.
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (const GLvoid*)bgra);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	// prepare the result.
	GrTexture* result(E_NEW(ctx, GrTexture)(this));
	result->_userdata = (void*)(size_t)id;

	// copy the texture data.
	result->_width = width;
	result->_height = height;
	result->_pixels = BufCpy(ctx, bgra, 4*width*height);

	// return the result.
	return result;
}


//===================
// GL2Driver::OnDestroyTexture
//===================
void		GL2Driver::OnDestroyTexture(GrTexture& texture)
{
	GLuint gl2id((GLuint)(size_t)texture._userdata);
	if (gl2id != (GLuint)-1)
		glDeleteTextures(1, &gl2id);
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

