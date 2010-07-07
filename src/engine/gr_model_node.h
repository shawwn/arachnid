//========================================================================
//	file:		gr_model_node.h
//	author:		Shawn Presser 
//	date:		7/2/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
#include "m_mat44.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrMesh;
class GrMaterial;
struct GrModelNode_stl;
//========================================================================

//========================================================================
// GrModelNode
//========================================================================
class ENGINE_API GrModelNode
{
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
	wstring				_name;
	MMat44				_transform;
	GrMesh*				_mesh;
	GrModelNode_stl*	_stl;

public:
	GrModelNode(const wstring& name);
	~GrModelNode();

	const wstring&		GetName() const							{ return _name; }

	const MMat44&		GetTransform() const					{ return _transform; }
	void				SetTransform(const MMat44& m)			{ _transform = m; }

	GrMesh*				GetMesh() const							{ return _mesh; }

	// deletes any previous mesh and stores the specified mesh.
	void				SetMesh(GrMesh* mesh);

	// ALWAYS allows the child to be added, unless it is NULL.
	void				AddChildModelNode(GrModelNode* node);

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
};
//========================================================================


