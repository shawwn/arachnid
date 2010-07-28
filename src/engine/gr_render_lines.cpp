//========================================================================
//	file:		gr_render_lines.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_render_lines.h"

// graphics headers.
#include "gr_model.h"
#include "gr_model_node.h"
#include "gr_mesh.h"
#include "gr_driver.h"

// math headers.
#include "m_transform.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
struct LineEntry
{
	SVec3		startPos;
	uint		startColor;

	SVec3		endPos;
	uint		endColor;
};

struct GrRenderLines_impl
{
	vector<LineEntry>			lines;
};
//========================================================================

//========================================================================
// GrRenderLines
//========================================================================

//===================
// GrRenderLines::GrRenderLines
//===================
GrRenderLines::GrRenderLines()
: _impl(E_NEW("render_lines", GrRenderLines_impl))
, _startCol(SVec4(1.0f, 1.0f, 1.0f, 1.0f).GetColor())
, _endCol(SVec4(1.0f, 1.0f, 1.0f, 1.0f).GetColor())
{
}


//===================
// GrRenderLines::GrRenderLines
//===================
GrRenderLines::~GrRenderLines()
{
	E_DELETE("render_lines", _impl);
}


//===================
// GrRenderLines::Reset
//===================
void				GrRenderLines::Reset()
{
	_impl->lines.clear();
}


//===================
// GrRenderLines::AddNormals
//===================
void				GrRenderLines::AddNormals(GrModel& model, float scale)
{
	// add the hierarchy.
	AddNormals(model.GetRoot(), scale);

	// recurse to children.
	uint numChildren(model.GetNumChildModels());
	for (uint i = 0; i < numChildren; ++i)
	{
		AddNormals(*model.GetChildModel(i), scale);
	}
}


//===================
// GrRenderLines::AddModelNode
//===================
void				GrRenderLines::AddNormals(GrModelNode& modelNode, float scale)
{
	GrMesh* mesh(modelNode.GetMesh());
	if (mesh == NULL)
		return;

	const MTransform& worldXform(modelNode.GetWorld());

	GrSkin* skin(mesh->GetSkin());
	uint meshChannels(skin->GetMeshChannels());
	uint numVerts(skin->GetNumVerts());

	const SVec3* positions(skin->GetPositions());

	// render tangents.
	if (meshChannels & MESH_TANGENTS)
	{
		SetColor(SVec4(1.0f, 0.0f, 0.0f, 1.0f).GetColor());
		const SVec3* tangents(skin->GetTangents());
		for (uint i = 0; i < numVerts; ++i)
			AddLine(worldXform, positions[i], positions[i] + scale*tangents[i]);
	}

	// render binormals.
	if (meshChannels & MESH_BINORMALS)
	{
		SetColor(SVec4(0.0f, 0.0f, 1.0f, 1.0f).GetColor());
		const SVec3* binormals(skin->GetBinormals());
		for (uint i = 0; i < numVerts; ++i)
			AddLine(worldXform, positions[i], positions[i] + scale*binormals[i]);
	}

	// render normals.
	if (meshChannels & MESH_NORMALS)
	{
		SetColor(SVec4(0.0f, 1.0f, 0.0f, 1.0f).GetColor());
		const SVec3* normals(skin->GetNormals());
		for (uint i = 0; i < numVerts; ++i)
			AddLine(worldXform, positions[i], positions[i] + scale*normals[i]);
	}

	// recurse to children.
	uint numChildren(modelNode.GetNumChildModelNodes());
	for (uint i = 0; i < numChildren; ++i)
	{
		AddNormals(*modelNode.GetChildModelNode(i), scale);
	}
}


//===================
// GrRenderLines::AddLine
//===================
void					GrRenderLines::AddLine(const MTransform& xform, const SVec3& startPos, const SVec3& endPos)
{
	_impl->lines.push_back(LineEntry());
	LineEntry& entry(_impl->lines.back());

	entry.startColor = _startCol;
	entry.endColor = _endCol;
	entry.startPos = startPos;
	entry.endPos = endPos;
}


//===================
// GrRenderLines::RenderLines
//===================
void					GrRenderLines::RenderLines(GrDriver* driver)
{
	if (_impl->lines.empty())
		return;

	driver->DrawLines((SColoredVertex*)&_impl->lines[0], _impl->lines.size());
}

//========================================================================
