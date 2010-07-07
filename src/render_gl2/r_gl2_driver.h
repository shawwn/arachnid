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
	static void		GlutOnMousePos(int x, int y);

	// renderer functions.
	void			ApplyCamera(const GrCamera& cam);
	void			RenderModelNode(const GrModelNode& node);
	void			RenderModel(const GrModel& model);

public:
	GL2Driver(int windowWidth, int windowHeight, const wstring& windowTitle);
	~GL2Driver();

	bool			HasFatalError() const				{ return _fatalError; }

	bool			BeginFrame();
	void			EndFrame();

	void			SetMousePos(int x, int y);

	GrMesh*			CreateMesh(const wchar_t* ctx,
		const SVec3* positions, const SVec2* texcoords, uint numVerts,
		const TriIdx* triangles, uint numTris);
	void			OnDestroyMesh(GrMesh& mesh) { }

	GrTexture*		CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height);
	void			OnDestroyTexture(GrTexture& texture);
};
