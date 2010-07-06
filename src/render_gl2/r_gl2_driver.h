//========================================================================
//	file:		gl2_driver.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================

// graphics headers.
#include "../engine/gr_types.h"
#include "../engine/gr_driver.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrScene;
class GrCamera;
class GrModel;
class GrModelNode;
class GrMesh;
class GrMaterial;
//========================================================================

//========================================================================
// GL2Driver
//========================================================================
class GL2Driver : public Internal_GrDriver
{
private:
	bool		_fatalError;
	int			_winIdx;

	// GLUT callbacks.
	static void		GlutOnDisplay();
	static void		GlutOnWindowResized(int width, int height);

	// renderer functions.
	void			RenderModelNode(const GrModelNode& node);
	void			RenderModel(const GrModel& model);

public:
	GL2Driver(int windowWidth, int windowHeight, const wstring& windowTitle);
	~GL2Driver();

	bool			HasFatalError() const				{ return _fatalError; }

	bool			BeginFrame();
	void			EndFrame();

	GrMesh*			CreateMesh(
		const SVec3* positions, const SVec2* texcoords, uint numVerts,
		const TriIdx* triangles, uint numTris);
};
