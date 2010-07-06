//========================================================================
//	file:		gr_model_node.cpp
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_model_node.h"
#include "gr_mesh.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
typedef vector<GrModelNode*>				ModelNodeVec;
typedef vector<GrModelNode::SMeshRange>		MeshRangeVec;
//========================================================================

//========================================================================
// GrModelNode_stl
//========================================================================
struct GrModelNode_stl
{
	ModelNodeVec		children;
	MeshRangeVec		meshRanges;
};


//===================
// GrModelNode::GrModelNode
//===================
GrModelNode::GrModelNode(const wstring& name)
: _name(name)
, _mesh(NULL)
, _stl(E_NEW("model_node", GrModelNode_stl))
{
}


//===================
// GrModelNode::~GrModelNode
//===================
GrModelNode::~GrModelNode()
{
	E_DELETE("modelNode", _mesh);
	E_DELETE("model_node", _stl);
}


//===================
// GrModelNode::SetMesh
//===================
void				GrModelNode::SetMesh(GrMesh* mesh)
{
	E_DELETE("modelNode", _mesh);
	_mesh = mesh;
}


//===================
// GrModelNode::AddChildModelNode
//===================
void				GrModelNode::AddChildModelNode(GrModelNode* node)
{
	E_VERIFY(node != NULL, return);
	_stl->children.push_back(node);
}


//===================
// GrModelNode::HasChildModelNode
//===================
bool				GrModelNode::HasChildModelNode(const wstring& name) const
{
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		if (child->GetName() == name)
			return true;
	}

	return false;
}


//===================
// GrModelNode::GetChildModelNode
//===================
GrModelNode*		GrModelNode::GetChildModelNode(const wstring& name) const
{
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		if (child->GetName() == name)
			return child;
	}

	return NULL;
}


//===================
// GrModelNode::GetChildModelNode
//===================
GrModelNode*		GrModelNode::GetChildModelNode(uint idx) const
{
	E_VERIFY(idx < _stl->children.size(), return NULL);
	return _stl->children[idx];
}


//===================
// GrModelNode::NumChildModelNodes
//===================
uint				GrModelNode::NumChildModelNodes() const
{
	return (uint)_stl->children.size();
}


//===================
// GrModelNode::GetMeshRange
//===================
GrModelNode::SMeshRange*	GrModelNode::GetMeshRange(uint idx)
{
	E_VERIFY(idx < _stl->meshRanges.size(), return NULL);
	return &_stl->meshRanges[idx];
}


//===================
// GrModelNode::AddMeshRange
//===================
void				GrModelNode::AddMeshRange(const SMeshRange& range)
{
	_stl->meshRanges.push_back(range);
}


//===================
// GrModelNode::NumMeshRanges
//===================
uint				GrModelNode::NumMeshRanges() const
{
	return (uint)_stl->meshRanges.size();
}


