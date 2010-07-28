//========================================================================
//	file:		gr_render_lines.h
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
struct GrRenderLines_impl;
//========================================================================

//========================================================================
// GrRenderLines
//========================================================================
class GrRenderLines
{
private:
	GrRenderLines_impl*	_impl;

	uint				_startCol;
	uint				_endCol;

public:
	GrRenderLines();
	~GrRenderLines();

	void					Reset();
	void					AddNormals(GrModel& model, float scale);
	void					AddNormals(GrModelNode& modelNode, float scale);
	void					SetColor(const uint& startColor, const uint& endColor)	{ _startCol = startColor; _endCol = endColor; }
	void					SetColor(const uint& color)								{ _startCol = color; _endCol = color; }
	void					AddLine(const MTransform& xform, const SVec3& startPos, const SVec3& endPos);
	void					RenderLines(GrDriver* driver);
};
//========================================================================
