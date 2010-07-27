//========================================================================
//	file:		gr_render_ambient.h
//	author:		Shawn Presser 
//	date:		7/27/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
class GrMesh;
class GrModel;
class GrModelNode;
class GrDriver;
struct GrRenderAmbient_impl;
//========================================================================

//========================================================================
// GrRenderAmbient
//========================================================================
class GrRenderAmbient
{
private:
	GrRenderAmbient_impl*	_impl;
public:
	GrRenderAmbient();
	~GrRenderAmbient();

	void					Reset();
	void					AddModel(GrModel& model);
	void					AddModelNode(GrModelNode& modelNode);
	void					RenderAmbient(GrDriver* driver);
};
//========================================================================
