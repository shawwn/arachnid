//========================================================================
//	file:		gr_model_node.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "gr_model_node.h"

// graphics headers.
#include "gr_model.h"
#include "gr_mesh.h"

// debug headers.
#include "e_filemanager.h"
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
// GrModelNode::Update
//===================
void				GrModelNode::Update(const MTransform& parentWorld)
{
	E_ASSERT(_owner == NULL || _owner->IsDirty());

	_world = parentWorld * _local;
	_skinningXform = (_world * _invBind);

	// recurse to children.
	for (ModelNodeVec::iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->Update(_world);
	}
}


//===================
// GrModelNode::AddToSkeleton
//===================
void				GrModelNode::AddToSkeleton(const MTransform** skeleton)
{
	if (_boneIdx >= 0)
		skeleton[_boneIdx] = &this->GetSkinningXForm();

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->AddToSkeleton(skeleton);
	}
}


//===================
// GrModelNode::CountChildren
//===================
uint				GrModelNode::CountChildren()
{
	uint count((uint)_stl->children.size());

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		count += child->CountChildren();
	}

	return count;
}


//===================
// GrModelNode::PrintChain
//===================
void				GrModelNode::PrintChain(wstring& str) const
{
	str.append(_name);

	if (_parent != NULL)
	{
		str.append(_T(" -> "));
		_parent->PrintChain(str);
	}
}


//===================
// GrModelNode::SetDirty
//===================
void				GrModelNode::SetDirty()
{
	if (_owner != NULL)
		_owner->SetDirty();
}


//===================
// GrModelNode::SetOwner
//===================
void				GrModelNode::SetOwner(GrModel* owner)
{
	_owner = owner;

	// recurse to children.
	for (ModelNodeVec::iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->SetOwner(owner);
	}
}


//===================
// GrModelNode::GrModelNode
//===================
GrModelNode::GrModelNode(const wstring& name)
: _name(name)
, _parent(NULL)
, _mesh(NULL)
, _stl(E_NEW("model_node", GrModelNode_stl))
, _boneIdx(-1)
, _skeleton(NULL)
, _owner(NULL)
{
}


//===================
// GrModelNode::~GrModelNode
//===================
GrModelNode::~GrModelNode()
{
	ClearChildModelNodes();
	E_DELETE("model_node", _mesh);
	E_DELETE("model_node", _stl);
	E_DELETE_ARRAY("model_node", _skeleton);
}


//===================
// GrModelNode::PrintDebug
//===================
void				GrModelNode::PrintDebug(EFile* file, uint indent) const
{
	E_VERIFY(file != NULL && file->IsOpen(), return);
	E_VERIFY(file->HasMode(FILE_WRITE) && file->HasMode(FILE_TEXT), return);

	E_ASSERT(_owner != NULL);
	if (_owner != NULL)
		_owner->UpdateTransforms();

	// set the indentation level.
	file->SetIndent(indent);

	// write the header.
	file->BeginTextHeader();
	file->WriteLine(_TP("Model Node [%s]", _name.c_str()));
	if (_parent != NULL)
	{
		wstring str;
		PrintChain(str);
		file->WriteLine(str);
	}
	file->EndTextHeader();

	// write the info.
	file->WriteLine(_TP("invbind           %s",	TO_STR(_invBind).c_str()));
	file->WriteLine(_TP("inv(invBind)      %s",	TO_STR(_invBind.Opposite()).c_str()));
	file->WriteLine(_TP("local             %s",	TO_STR(_local).c_str()));
	file->WriteLine(_TP("world             %s",	TO_STR(_world).c_str()));
	file->WriteLine(_TP("skinning          %s",	TO_STR(_skinningXform).c_str()));
	file->WriteBlankLine();

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->PrintDebug(file, indent + 1);
	}
}


//===================
// GrModelNode::SetWorld
//===================
void				GrModelNode::SetWorld(const MTransform& world)
{
	// if we don't have a parent, then set our local transform.
	if (_parent == NULL)
	{
		_local = world;
		return;
	}

	// get our parent's world transform.
	const MTransform& parentXform(_parent->GetWorld());

	// invert it.
	MTransform invParentXform(parentXform.Opposite());

	// set our new worldspace transform.
	_local = invParentXform * world;
	_world = world;
	SetDirty();
	
	E_ASSERT((parentXform * _local).Compare(_world, 0.01f));
	E_ASSERT(invParentXform.Opposite().Compare(parentXform, 0.01f));
}


//===================
// GrModelNode::ResetHierarchyToLocal
//===================
void				GrModelNode::ResetHierarchyToLocal(const MTransform& m)
{
	SetLocal(m);

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->ResetHierarchyToLocal(m);
	}
}


//===================
// GrModelNode::ResetHierarchyToWorld
//===================
void				GrModelNode::ResetHierarchyToWorld(const MTransform& m)
{
	SetWorld(m);

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->ResetHierarchyToWorld(m);
	}
}


//===================
// GrModelNode::ResetHierarchyToBindPose
//===================
void				GrModelNode::ResetHierarchyToBindPose(const MTransform& parentInvBind)
{
	_local = (parentInvBind * _bind);
	_world = _bind;
	SetDirty();

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->ResetHierarchyToBindPose(_invBind);
	}
}


//===================
// GrModelNode::RotateHierarchy
//===================
void				GrModelNode::RotateHierarchy(const MMat33& deltaRot)
{
	const MTransform& world(GetWorld());
	MTransform m(deltaRot * world.GetRot(), world.GetTrans());
	SetWorld(m);
}


//===================
// GrModelNode::TransformHierarchy
//===================
void				GrModelNode::TransformHierarchy(const MTransform& delta)
{
	//SetWorld(delta * GetWorld());
}


//===================
// GrModelNode::GetWorld
//===================
const MTransform&	GrModelNode::GetWorld()
{
	E_ASSERT(_owner != NULL);
	if (_owner != NULL)
		_owner->UpdateTransforms();
	return _world;
}


//===================
// GrModelNode::GetSkinningXForm
//===================
const MTransform&	GrModelNode::GetSkinningXForm()
{
	E_ASSERT(_owner != NULL);
	if (_owner != NULL)
		_owner->UpdateTransforms();
	return _skinningXform;
}


//===================
// GrModelNode::SetMesh
//===================
void				GrModelNode::SetMesh(GrMesh* mesh)
{
	E_DELETE("model_node", _mesh);
	_mesh = mesh;
}


//===================
// GrModelNode::AddChildModelNode
//===================
uint				GrModelNode::AddChildModelNode(GrModelNode* node)
{
	E_VERIFY(node != NULL, return uint(-1));

	// set child params.
	node->_parent = this;
	node->SetOwner(_owner);
	_stl->children.push_back(node);
	SetDirty();

	return uint(_stl->children.size() - 1);
}


//===================
// GrModelNode::Find
//===================
GrModelNode*		GrModelNode::Find(const wstring& name)
{
	if (name == _name)
		return this;

	// recurse to children.
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		GrModelNode* found(child->Find(name));
		if (found != NULL)
			return found;
	}

	return NULL;
}


//===================
// GrModelNode::ClearChildModelNodes
//===================
void				GrModelNode::ClearChildModelNodes()
{
	for (ModelNodeVec::const_iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		E_DELETE("model_node", child);
	}
	_stl->children.clear();
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
uint				GrModelNode::GetNumChildModelNodes() const
{
	return (uint)_stl->children.size();
}


//===================
// GrModelNode::GetMeshRange
//===================
GrModelNode::SMeshRange*	GrModelNode::GetMeshRange(uint idx) const
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
uint				GrModelNode::GetNumMeshRanges() const
{
	return (uint)_stl->meshRanges.size();
}


//===================
// GrModelNode::GetSkeleton
//===================
const MTransform**	GrModelNode::GetSkeleton()
{
	if (_skeleton == NULL)
	{
		uint numBones(CountChildren());
		const MTransform** skeleton = E_NEW_ARRAY("model_node", const MTransform*, numBones);
		BufZero(skeleton, numBones);
		_skeleton = skeleton;

		for (ModelNodeVec::iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
		{
			GrModelNode* child(*it);
			child->AddToSkeleton(_skeleton);
		}
	}

	return _skeleton;
}


//===================
// GrModelNode::Animate
//===================
void				GrModelNode::Animate()
{
	// if any meshes are animated, then deform them.
	if (_mesh != NULL)
	{
		GrSkin* skin(_mesh->GetSkin());
		if (skin->GetMeshChannels() & MESH_BONE_INFO)
		{
			skin->DeformVerts(GetSkeleton());
		}
	}

	// recurse to children.
	for (ModelNodeVec::iterator it(_stl->children.begin()), itEnd(_stl->children.end()); it != itEnd; ++it)
	{
		GrModelNode* child(*it);
		child->Animate();
	}
}

