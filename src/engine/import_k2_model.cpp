//========================================================================
//	file:		import_k2_model.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "import_k2_model.h"

// engine headers.
#include "e_file.h"

// graphics headers.
#include "gr_driver.h"
#include "gr_model.h"
#include "gr_model_node.h"
#include "gr_skin.h"

// math headers.
#include "m_vec3.h"
#include "m_mat33.h"
//========================================================================

//========================================================================
// K2Mesh
//========================================================================
struct K2Mesh
{
	int			index;
	int			mode;
	int			numVertices;
	SVec3		bbMin;
	SVec3		bbMax;
	int			bone;

	byte		nameSize;
	byte		name2Size;
	char		name[512];

	SVec3*		positions;
	SVec3*		normals;
	SVec2*		texcoords;

	SVec3*		tangents[8];

	int			numTriangles;
	TriIdx*		triangles;

	K2Mesh()
		: numVertices(0)
		, numTriangles(0)
		, positions(NULL)
		, normals(NULL)
		, texcoords(NULL)
		, triangles(NULL)
	{
		for (uint i = 0; i < 8; ++i)
			tangents[i] = NULL;
	}

	~K2Mesh()
	{
		E_DELETE_ARRAY("k2mesh", positions);
		E_DELETE_ARRAY("k2mesh", normals);
		E_DELETE_ARRAY("k2mesh", texcoords);
		E_DELETE_ARRAY("k2mesh", triangles);

		for (uint i = 0; i < 8; ++i)
			E_DELETE_ARRAY("k2mesh", tangents[i]);
	}
};
//========================================================================

//========================================================================
// K2Bone
//========================================================================
struct K2Bone
{
	int					parentIndex;
	GrModelNode*		node;

	K2Bone(int parentIndex, GrModelNode* node)
		: parentIndex(parentIndex)
		, node(node)
	{
	}
};
typedef map<int, K2Bone>	BoneMap;
//========================================================================

//========================================================================
// ImportK2Model_stl
//========================================================================
struct ImportK2Model_stl
{
	vector<K2Mesh>		meshes;
	vector<GrSkin*>		skins;
};


//===================
// ImportK2Model::ParseHeader
//===================
bool			ImportK2Model::ParseHeader(GrDriver& driver, EFile* file, int blockSize)
{
	int version = file->ReadInt();
	E_VERIFY(version == 3, return false);

	_numMeshes = file->ReadInt();
	_numSprites = file->ReadInt();
	_numSurfaces = file->ReadInt();
	_numBones = file->ReadInt();
	_bbMin = file->ReadMVec3();
	_bbMax = file->ReadMVec3();

	E_VERIFY(_numMeshes > 0, return false);
	E_VERIFY(_numBones >= 0, return false);

	_stl->meshes.reserve(_numMeshes);
	if (_numBones > 1)
		_stl->skins.resize(_numMeshes);

	return true;
}


//===================
// ImportK2Model::ParseMesh
//===================
bool			ImportK2Model::ParseMesh(GrDriver& driver, EFile* file, int blockSize)
{
	_stl->meshes.push_back(K2Mesh());
	K2Mesh& block(*_stl->meshes.rbegin());

	block.index = file->ReadInt();
	block.mode = file->ReadInt();
	block.numVertices = file->ReadInt();
	block.bbMin = file->ReadSVec3();
	block.bbMax = file->ReadSVec3();
	block.bone = file->ReadInt();

	block.nameSize = file->ReadByte();
	block.name2Size = file->ReadByte();
	file->ReadArray(block.name, block.nameSize + 1);

	return true;
}


//===================
// ImportK2Model::ParseVertices
//===================
bool			ImportK2Model::ParseVertices(GrDriver& driver, EFile* file, int blockSize)
{
	uint meshIndex(file->ReadInt());
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	E_DELETE_ARRAY("k2mesh", k2mesh.positions);
	k2mesh.positions = E_NEW_ARRAY("k2mesh", SVec3, k2mesh.numVertices);

	float* pos = (float*)k2mesh.positions;
	for (int i = 0; i < k2mesh.numVertices; ++i, pos += 3)
	{
		pos[0] = file->ReadFloat();
		pos[1] = file->ReadFloat();
		pos[2] = file->ReadFloat();
	}

	return true;
}


//===================
// ImportK2Model::ParseNormals
//===================
bool			ImportK2Model::ParseNormals(GrDriver& driver, EFile* file, int blockSize)
{
	uint meshIndex(file->ReadInt());
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	E_DELETE_ARRAY("k2mesh", k2mesh.normals);
	k2mesh.normals = E_NEW_ARRAY("k2mesh", SVec3, k2mesh.numVertices);

	float* normals = (float*)k2mesh.normals;
	for (int i = 0; i < k2mesh.numVertices; ++i, normals += 3)
	{
		normals[0] = file->ReadFloat();
		normals[1] = file->ReadFloat();
		normals[2] = file->ReadFloat();
	}

	return true;
}


//===================
// ImportK2Model::ParseTangents
//===================
bool			ImportK2Model::ParseTangents(GrDriver& driver, EFile* file, int blockSize)
{
	uint meshIndex(file->ReadInt());
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	int channel(file->ReadInt());
	E_VERIFY(channel >= 0 && channel < 8, return false);

	E_DELETE_ARRAY("k2mesh", k2mesh.tangents[channel]);
	k2mesh.tangents[channel] = E_NEW_ARRAY("k2mesh", SVec3, k2mesh.numVertices);

	float* tangents = (float*)k2mesh.tangents[channel];
	for (int i = 0; i < k2mesh.numVertices; ++i, tangents += 3)
	{
		tangents[0] = file->ReadFloat();
		tangents[1] = file->ReadFloat();
		tangents[2] = file->ReadFloat();
	}

	return true;
}


//===================
// ImportK2Model::ParseTexcoords
//===================
bool			ImportK2Model::ParseTexcoords(GrDriver& driver, EFile* file, int blockSize)
{
	uint meshIndex(file->ReadInt());
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	uint channel(file->ReadInt());
	if (channel != 0)
		return true;

	E_DELETE_ARRAY("k2mesh", k2mesh.texcoords);
	k2mesh.texcoords = E_NEW_ARRAY("k2mesh", SVec2, k2mesh.numVertices);
	float* uv = (float*)k2mesh.texcoords;

	for (int i = 0; i < k2mesh.numVertices; ++i, uv += 2)
	{
		uv[0] = file->ReadFloat();
		uv[1] = file->ReadFloat();
	}

	return true;
}


//===================
// ImportK2Model::ParseBones
//===================
bool			ImportK2Model::ParseBones(GrDriver& driver, EFile* file, int blockSize)
{
	const int boneSize
		= sizeof(uint32)	// parentIndex

		+ sizeof(float)		// inverse base matrix x translation
		+ 3*sizeof(float)	// inverse base matrix x basis

		+ sizeof(float)		// inverse base matrix y translation
		+ 3*sizeof(float)	// inverse base matrix y basis

		+ sizeof(float)		// inverse base matrix z translation
		+ 3*sizeof(float)	// inverse base matrix z basis

		+ sizeof(float)		// base matrix x translation
		+ 3*sizeof(float)	// base matrix x basis

		+ sizeof(float)		// base matrix y translation
		+ 3*sizeof(float)	// base matrix y basis

		+ sizeof(float)		// base matrix z translation
		+ 3*sizeof(float);	// base matrix z basis

	GrModelNode& rootNode(_model->GetRoot());
	rootNode.ClearChildModelNodes();

	BoneMap boneMap;

	int curBoneIndex(0);
	do 
	{
		int parentIndex = file->ReadInt();

		// read inv base.
		MTransform invBaseXform(file->ReadTransform(false));

		// read base.
		MTransform baseXform(file->ReadTransform(false));

		// read the name of the bone.
		byte boneNameSize = file->ReadByte();
		char boneName[256];
		file->ReadArray(boneName, boneNameSize + 1);
		wstring boneNameStr(StringToWString(boneName));

		// create the bone.
		GrModelNode* boneNode(E_NEW("model_node", GrModelNode)(boneNameStr));
		boneNode->SetBoneIdx(curBoneIndex);
		boneNode->SetInvBind(invBaseXform, baseXform);
		boneMap.insert(std::make_pair(curBoneIndex, K2Bone(parentIndex, boneNode)));

		// advance to the next bone.
		blockSize -= boneSize + boneNameSize + 1;
		++curBoneIndex;
	} while (blockSize > boneSize);

	// build the skeleton.
	for (BoneMap::iterator it(boneMap.begin()), itEnd(boneMap.end()); it != itEnd; ++it)
	{
		int boneIndex(it->first);
		int boneParentIndex(it->second.parentIndex);
		GrModelNode* boneNode(it->second.node);
		E_VERIFY(boneNode != NULL, continue);

		// if the bone has no parent, then add it to the root bone.
		if (boneParentIndex < 0)
		{
			rootNode.AddChildModelNode(boneNode);
			continue;
		}

		// otherwise, find the parent.
		BoneMap::iterator findIt(boneMap.find(boneParentIndex));
		if (findIt == boneMap.end())
		{
			E_WARN("import_k2_model", _TS("ImportK2Model::ParseBones: Could not find parent bone!"));
			continue;
		}

		GrModelNode* parentBoneNode(findIt->second.node);
		E_VERIFY(parentBoneNode != NULL, continue);
		parentBoneNode->AddChildModelNode(boneNode);
	}
	boneMap.clear();

	rootNode.ResetHierarchyToBindPose();

	return true;
}


//===================
// ImportK2Model::ParseSkin
//===================
bool				ImportK2Model::ParseSkin(GrDriver& driver, EFile* file, int blockSize)
{
	int meshIndex = file->ReadInt();
	E_VERIFY(meshIndex >= 0 && meshIndex < (int)_stl->skins.size(), return false);

	int numVerts = file->ReadInt();
	E_VERIFY(numVerts > 0, return false);

	K2Mesh& mesh = _stl->meshes[meshIndex];
	GrSkin*& skin = _stl->skins[meshIndex];
	E_VERIFY(skin == NULL, return false);

	skin = E_NEW("import_k2_model", GrSkin)((uint)numVerts);

	vector<float> weights;
	vector<int> indices;

	weights.resize(20);
	indices.resize(20);

	for (int vertIdx = 0; vertIdx < numVerts; ++vertIdx)
	{
		E_ASSERT(mesh.positions != NULL && mesh.normals != NULL && mesh.texcoords != NULL);
		skin->StartVert(mesh.positions[vertIdx], mesh.texcoords[vertIdx]);
		skin->AddNormals(mesh.normals[vertIdx]);

		if (mesh.tangents[0] && mesh.tangents[1])
			skin->AddTangents(mesh.tangents[0][vertIdx], mesh.tangents[1][vertIdx]);

		if (mesh.tangents[0] && mesh.normals)
		{
			SVec3 binormal(mesh.normals[vertIdx].Cross(mesh.tangents[0][vertIdx]));
			skin->AddTangents(mesh.tangents[0][vertIdx], binormal);
		}

		int numWeights = file->ReadInt();
		E_VERIFY(numWeights >= 0, continue);

		// ensure that there is room to store temp bone weights / indices.
		if ((int)weights.size() < numWeights)
			weights.resize(numWeights, 0.0f);
		if ((int)indices.size() < numWeights)
			indices.resize(numWeights, -1);

		// read the weights.
		for (int i = 0; i < numWeights; ++i)
			weights[i] = file->ReadFloat();

		// read the bone indices.
		for (int i = 0; i < numWeights; ++i)
		{
			indices[i] = file->ReadInt();
			E_VERIFY(indices[i] >= 0, return false);
		}

		// add the weights.
		for (int i = 0; i < numWeights; ++i)
			skin->AddBoneWeight((uint)indices[i], weights[i]);
	}

	int numTris(mesh.numTriangles);
	for (int triIdx = 0; triIdx < numTris; ++triIdx)
	{
		skin->AddTriangle(
			mesh.triangles[3*triIdx],
			mesh.triangles[3*triIdx + 1],
			mesh.triangles[3*triIdx + 2]);
	}

	return true;
}


//===================
// ImportK2Model::ParseTriangles
//===================
bool			ImportK2Model::ParseTriangles(GrDriver& driver, EFile* file, int blockSize)
{
	uint meshIndex(file->ReadInt());;
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	k2mesh.numTriangles = file->ReadInt();

	E_DELETE_ARRAY("k2mesh", k2mesh.texcoords);
	k2mesh.triangles = E_NEW_ARRAY("k2mesh", TriIdx, 3*k2mesh.numTriangles);
	TriIdx* indices = (TriIdx*)k2mesh.triangles;

	byte faceIndexSize(file->ReadByte());
	switch(faceIndexSize)
	{
	case 1:
		E_WARN("k2mesh", _T("ParseTriangles: triangle index size 4 not implemented."));
		break;

	case 2:
		for (int i = 0; i < k2mesh.numTriangles; ++i, indices += 3)
		{
			indices[0] = file->ReadShort();
			indices[1] = file->ReadShort();
			indices[2] = file->ReadShort();
		}
		break;

	case 4:
		E_WARN("k2mesh", _T("ParseTriangles: triangle index size 4 not implemented."));
		break;

	default:
		E_ASSERT(false);
	}

	return true;
}


//===================
// ImportK2Model::ImportK2Model
//===================
ImportK2Model::ImportK2Model(GrDriver& driver, GrMaterial* nullMat)
: _stl(E_NEW("importk2model", ImportK2Model_stl))
, _driver(driver)
, _file(NULL)
, _model(GrModel::Create(_T("k2mesh"),_T("k2mesh")))
, _nullMat(nullMat)
, _numMeshes(0)
, _numSprites(0)
, _numSurfaces(0)
, _numBones(0)
, _bbMin(MVec3::Zero)
, _bbMax(MVec3::Zero)
{
}


//===================
// ImportK2Model::~ImportK2Model
//===================
ImportK2Model::~ImportK2Model()
{
	E_DELETE("importk2model", _file);
	E_DELETE("importk2model", _stl);
}


//===================
// ImportK2Model::Read
//===================
bool			ImportK2Model::Read(EFile* file)
{
	_file = file;
	E_VERIFY(file != NULL && file->IsOpen(), return false);

	uint size(file->GetFileSize());

	// check the header.
	if (size < 4)
		return false;

	{
		byte sigS(file->ReadByte());
		byte sigM(file->ReadByte());
		byte sigD(file->ReadByte());
		byte sigL(file->ReadByte());

		if ((char)sigS != 'S' || (char)sigM != 'M' || (char)sigD != 'D' || (char)sigL != 'L')
			return false;
	}

	uivec delayedBlocks;
	char blockName[5];
	blockName[4] = 0;

	while (file->GetPos() + 4 + sizeof(int) < file->GetFileSize())
	{
		uint blockPos = file->GetPos();

		for (int i = 0; i < 4; ++i)
			blockName[i] = (char)file->ReadByte();

		int blockSize = file->ReadInt();

		uint filePos(file->GetPos());

		if (strncmp(blockName, "head", 4) == 0)
		{
			E_VERIFY(ParseHeader(_driver, _file, blockSize), return false);
		}
		if (strncmp(blockName, "mesh", 4) == 0)
		{
			E_VERIFY(ParseMesh(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "vrts", 4) == 0)
		{
			E_VERIFY(ParseVertices(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "nrml", 4) == 0)
		{
			E_VERIFY(ParseNormals(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "texc", 4) == 0)
		{
			E_VERIFY(ParseTexcoords(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "tang", 4) == 0)
		{
			E_VERIFY(ParseTangents(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "face", 4) == 0)
		{
			E_VERIFY(ParseTriangles(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "bone", 4) == 0)
		{
			E_VERIFY(ParseBones(_driver, _file, blockSize), return false);
		}
		else if (strncmp(blockName, "lnk1", 4) == 0)
		{
			delayedBlocks.push_back(blockPos);
		}

		file->Seek(filePos + blockSize);
	}

	// process delayed blocks.
	uint numDelayedBlocks((uint)delayedBlocks.size());
	for (uint i = 0; i < numDelayedBlocks; ++i)
	{
		_file->Seek(delayedBlocks[i]);

		for (int i = 0; i < 4; ++i)
			blockName[i] = (char)file->ReadByte();

		int blockSize = file->ReadInt();

		if (strncmp(blockName, "lnk1", 4) == 0)
		{
			E_VERIFY(ParseSkin(_driver, _file, blockSize), return false);
		}
	}
	delayedBlocks.clear();

	file->Close();

	// TODO:  Build all the meshes.  For now just build one.
	K2Mesh& k2mesh(_stl->meshes[0]);
	E_ASSERT(_stl->meshes.size() == 1);

	GrSkin* skin = _stl->skins[0];
	E_ASSERT(skin != NULL);

	GrMesh* mesh(_driver.CreateMesh(_T("k2mesh"), skin));
	if (mesh != NULL)
	{
		GrModelNode& modelNode(_model->GetRoot());
		modelNode.SetMesh(mesh);
		modelNode.AddMeshRange(GrModelNode::SMeshRange(0, k2mesh.numTriangles, _nullMat));
	}

#if E_PRINT_DEBUG_INFO
	_model->PrintDebug();
#endif

	return true;
}

