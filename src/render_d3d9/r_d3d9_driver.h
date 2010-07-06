//========================================================================
//	file:		d3d9_driver.h
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
// D3D9Driver
//========================================================================
class D3D9Driver : public Internal_GrDriver
{
private:
	bool		_fatalError;
	int			_winIdx;

public:
	D3D9Driver(int windowWidth, int windowHeight, const wstring& windowTitle);
	~D3D9Driver();

	bool		HasFatalError() const				{ return _fatalError; }

	bool		BeginFrame();
	void		Render(const GrScene& scene, const GrCamera& camera);
	void		EndFrame();

	GrMesh*		CreateMesh(
		const SVec3* positions, const SVec2* texcoords, uint numVerts,
		const TriIdx* triangles, uint numTris)		{ return NULL; }
};