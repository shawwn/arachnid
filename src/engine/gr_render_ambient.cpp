//========================================================================
//	file:		gr_render_ambient.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_render_ambient.h"

// graphics headers.
#include "gr_model.h"
#include "gr_model_node.h"
#include "gr_mesh.h"
#include "gr_driver.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
struct MeshEntry
{
	MTransform					xform;
	GrMesh*						mesh;
	GrModelNode::SMeshRange		range;

	bool	operator <(const MeshEntry& rhs) const
	{
		if (range.material != rhs.range.material)
			return range.material < rhs.range.material;

		if (mesh != rhs.mesh)
			return mesh < rhs.mesh;

		return range.triStart < rhs.range.triStart;
	}
};

struct GrRenderAmbient_impl
{
	vector<MeshEntry>			meshes;
};
//========================================================================

//========================================================================
// GrRenderAmbient
//========================================================================

//===================
// GrRenderAmbient::GrRenderAmbient
//===================
GrRenderAmbient::GrRenderAmbient()
: _impl(E_NEW("render_ambient", GrRenderAmbient_impl))
{
}


//===================
// GrRenderAmbient::GrRenderAmbient
//===================
GrRenderAmbient::~GrRenderAmbient()
{
	E_DELETE("render_ambient", _impl);
}


//===================
// GrRenderAmbient::Reset
//===================
void				GrRenderAmbient::Reset()
{
	_impl->meshes.clear();
}


//===================
// GrRenderAmbient::AddModel
//===================
void				GrRenderAmbient::AddModel(GrModel& model)
{
	// add the hierarchy.
	AddModelNode(model.GetRoot());

	// recurse to children.
	uint numChildren(model.GetNumChildModels());
	for (uint i = 0; i < numChildren; ++i)
	{
		AddModel(*model.GetChildModel(i));
	}
}


//===================
// GrRenderAmbient::AddModelNode
//===================
void				GrRenderAmbient::AddModelNode(GrModelNode& modelNode)
{
	const MTransform& worldXform(modelNode.GetWorld());

	// add each mesh range.
	uint numRanges(modelNode.GetNumMeshRanges());
	for (uint i = 0; i < numRanges; ++i)
	{
		GrMesh* mesh(modelNode.GetMesh());
		GrModelNode::SMeshRange* range(modelNode.GetMeshRange(i));
		E_VERIFY(mesh != NULL && range != NULL && range->material != NULL, continue);

		_impl->meshes.push_back(MeshEntry());
		MeshEntry& entry(_impl->meshes.back());

		entry.xform = worldXform;
		entry.mesh = mesh;
		entry.range = *range;
	}

	// recurse to children.
	uint numChildren(modelNode.GetNumChildModelNodes());
	for (uint i = 0; i < numChildren; ++i)
	{
		AddModelNode(*modelNode.GetChildModelNode(i));
	}
}


//===================
// GrRenderAmbient::RenderAmbient
//===================
void					GrRenderAmbient::RenderAmbient(GrDriver* driver)
{
	// sort.
	std::sort(_impl->meshes.begin(), _impl->meshes.end());

	GrMaterial* curMaterial(NULL);

	size_t count(_impl->meshes.size());
	for (size_t i = 0; i < count; ++i)
	{
		MeshEntry& entry(_impl->meshes[i]);

		if (entry.range.material != curMaterial)
		{
			curMaterial = entry.range.material;
			driver->ApplyMaterial(curMaterial, MAT_PASS_AMBIENT);
		}

		driver->DrawMeshRange(entry.xform, entry.mesh, entry.range.triStart, entry.range.triCount);
	}
}

//========================================================================
