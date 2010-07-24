//========================================================================
//	file:		gr_model_node.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_transform.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class EFile;
class GrMesh;
class GrMaterial;
class GrModel;
struct GrModelNode_stl;
//========================================================================

//========================================================================
// GrModelNode
//========================================================================
class ENGINE_API GrModelNode
{
	friend class GrModel;

public:
	//========================================================================
	// SMeshRange
	//========================================================================
	struct ENGINE_API SMeshRange
	{
		SMeshRange()
			: triStart(0)
			, triCount(0)
			, material(NULL)
		{
		}

		SMeshRange(uint triStart, uint triCount, GrMaterial* material)
			: triStart(triStart)
			, triCount(triCount)
			, material(material)
		{
		}

		uint		triStart;
		uint		triCount;
		GrMaterial*	material;
	};

private:
	wstring						_name;

	// represents the bone's current transform relative to itself.
	MTransform					_local;

	// a transform to move a point in space from modelspace to bone space.
	MTransform					_invBind;
	MTransform					_bind;

	// the node's modelspace transform.
	mutable MTransform			_world;

	// the transform to skin a vertex.
	mutable MTransform			_skinningXform;

	// the parent bone.
	GrModelNode*				_parent;

	// the owner model.
	GrModel*					_owner;

	// any meshes attached to the bone.
	GrMesh*						_mesh;
	GrModelNode_stl*			_stl;

	// the index of the bone.
	int							_boneIdx;
	const MTransform**			_skeleton;

	void				Update(const MTransform& parentWorld);

	void				AddToSkeleton(const MTransform** skeleton);
	uint				CountChildren();

	void				PrintChain(wstring& str) const;

	void				SetDirty();

	void				SetOwner(GrModel* owner);
public:
	GrModelNode(const wstring& name);
	~GrModelNode();

	void				PrintDebug(EFile* file, uint indent = 0) const;

	const wstring&		GetName() const							{ return _name; }
	GrModelNode*		GetParent() const						{ return _parent; }
	GrModel*			GetOwner() const						{ return _owner; }

	const MTransform&	GetInvBind() const						{ return _invBind; }
	const MTransform&	GetBind() const							{ return _bind; }

	void				SetInvBind(const MTransform& invBind, const MTransform& bind)
	{
		_invBind = invBind;
		_bind = invBind.Opposite();
		E_ASSERT(_bind.Compare(bind, 0.01f));
		SetDirty();
	}

	void				MultLocal(const MTransform& local)		{ _local = (local * _local); SetDirty(); }
	void				SetLocal(const MTransform& local)		{ _local = local; SetDirty(); }
	void				SetWorld(const MTransform& world);

	// each node in the hierarchy to the specified transform.
	void				ResetHierarchyToLocal(const MTransform& m);
	void				ResetHierarchyToWorld(const MTransform& m);
	
	// resets the hierarchy to its initial (bind) pose.
	void				ResetHierarchyToBindPose(const MTransform& parentInvBind = MTransform::Identity);

	// rotates the node along with its children.
	void				RotateHierarchy(const MMat33& deltaRot);

	void				TransformHierarchy(const MTransform& delta);

	const MTransform&	GetLocal() const						{ return _local; }
	const MTransform&	GetWorld();
	const MTransform&	GetSkinningXForm();

	int					GetBoneIdx() const						{ return _boneIdx; }
	void				SetBoneIdx(uint idx)					{ _boneIdx = (int)idx; }

	GrMesh*				GetMesh() const							{ return _mesh; }

	// deletes any previous mesh and stores the specified mesh.
	void				SetMesh(GrMesh* mesh);

	// ALWAYS allows the child to be added, unless it is NULL.
	//	returns the index of the child.
	uint				AddChildModelNode(GrModelNode* node);

	// tries to find the child.
	GrModelNode*		Find(const wstring& name);

	// clears all children.
	void				ClearChildModelNodes();

	// searches for the first instance of a child node with the specified name.
	bool				HasChildModelNode(const wstring& name) const;
	GrModelNode*		GetChildModelNode(const wstring& name) const;

	// provides array access to the child nodes.
	GrModelNode*		GetChildModelNode(uint idx) const;
	uint				NumChildModelNodes() const;

	// provides array access to the child mesh ranges.
	SMeshRange*			GetMeshRange(uint idx) const;
	void				AddMeshRange(const SMeshRange& range);
	uint				NumMeshRanges() const;

	// builds a skeleton hierarchy out of the child node hierarchy.
	const MTransform**	GetSkeleton();
};
//========================================================================


