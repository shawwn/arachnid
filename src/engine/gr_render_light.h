//========================================================================
//	file:		gr_render_light.h
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
class GrLight;
class GrDriver;
struct GrRenderLight_impl;
//========================================================================

//========================================================================
// GrRenderLight
//========================================================================
class GrRenderLight
{
private:
	GrRenderLight_impl*	_impl;
public:
	GrRenderLight();
	~GrRenderLight();

	void					Reset(const GrLight& light);
	void					AddModel(GrModel& model);
	void					AddModelNode(GrModelNode& modelNode);
	void					RenderLight(GrDriver* driver);
};
//========================================================================
