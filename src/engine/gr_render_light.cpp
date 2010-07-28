//========================================================================
//	file:		gr_render_light.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_render_light.h"

// graphics headers.
#include "gr_model.h"
#include "gr_model_node.h"
#include "gr_mesh.h"
#include "gr_light.h"
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

struct GrRenderLight_impl
{
	GrLight						light;
	vector<MeshEntry>			meshes;
};
//========================================================================

//========================================================================
// GrRenderLight
//========================================================================

//===================
// GrRenderLight::GrRenderLight
//===================
GrRenderLight::GrRenderLight()
: _impl(E_NEW("render_light", GrRenderLight_impl))
{
}


//===================
// GrRenderLight::GrRenderLight
//===================
GrRenderLight::~GrRenderLight()
{
	E_DELETE("render_light", _impl);
}


//===================
// GrRenderLight::Reset
//===================
void				GrRenderLight::Reset(const GrLight& light)
{
	_impl->light = light;
	_impl->meshes.clear();
}


//===================
// GrRenderLight::AddModel
//===================
void				GrRenderLight::AddModel(GrModel& model)
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
// GrRenderLight::AddModelNode
//===================
void				GrRenderLight::AddModelNode(GrModelNode& modelNode)
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
// GrRenderLight::RenderLight
//===================
void					GrRenderLight::RenderLight(GrDriver* driver)
{
	driver->ApplyLight(_impl->light);

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
			driver->ApplyMaterial(curMaterial, MAT_PASS_LIT);
		}

		driver->DrawMeshRange(entry.xform, entry.mesh, entry.range.triStart, entry.range.triCount);
	}
}

//========================================================================
