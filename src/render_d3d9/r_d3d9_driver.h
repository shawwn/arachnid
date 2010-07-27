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
struct D3D9Driver_impl;
//========================================================================

//========================================================================
// D3D9Driver
//========================================================================
class D3D9Driver : public Internal_GrDriver
{
private:
	bool				_fatalError;
	int					_winIdx;
	D3D9Driver_impl*	_impl;

	// renderer functions.
	void				ApplyCamera(const GrCamera& cam);
	void				RenderMeshNormals(GrMesh* mesh);
	void				RenderModelNode(GrModel& parent, GrModelNode& node);
	void				RenderModel(GrModel& model);

public:
	D3D9Driver(int windowWidth, int windowHeight, const wstring& windowTitle);
	~D3D9Driver();

	bool				HasFatalError() const				{ return _fatalError; }

	bool				BeginFrame();
	void				EndFrame();

	void				SetMousePos(int x, int y);

	// mesh management.
	GrMesh*				CreateMesh(const wchar_t* ctx, GrSkin* geometry);
	void				OnDestroyMesh(GrMesh& mesh);

	// texture management.
	GrTexture*			CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height);
	GrTexture*			CreateTexture(const wchar_t* ctx, const wstring& filePath);
	void				OnDestroyTexture(GrTexture& texture);

	// internal use only.
	void				OnResize(uint windowWidth, uint windowHeight);
	void				OnPaint();
	void				SetActive(bool active) { _isActive = active; }
};