//========================================================================
//	file:		import_k2_model.cpp
//	author:		Shawn Presser 
//	date:		7/7/10
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
	SVec2*		texcoords;

	int			numTriangles;
	TriIdx*		triangles;

	K2Mesh()
		: numVertices(0)
		, numTriangles(0)
		, positions(NULL)
		, texcoords(NULL)
		, triangles(NULL)
	{
	}

	~K2Mesh()
	{
		E_DELETE_ARRAY("k2mesh", positions);
		E_DELETE_ARRAY("k2mesh", texcoords);
		E_DELETE_ARRAY("k2mesh", triangles);
	}
};
//========================================================================

//========================================================================
// ImportK2Model_stl
//========================================================================
struct ImportK2Model_stl
{
	vector<K2Mesh>		meshes;
};

//===================
// ImportK2Model::ImportK2Model
//===================
ImportK2Model::ImportK2Model(GrDriver& driver, GrMaterial* nullMat)
: _stl(E_NEW("importk2model", ImportK2Model_stl))
, _driver(driver)
, _file(NULL)
, _model(GrModel::Create(_T("k2mesh"),_T("k2mesh")))
, _nullMat(nullMat)
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
// ImportK2Model::ParseMesh
//===================
bool			ImportK2Model::ParseMesh(GrDriver& driver, EFile* file, uint blockSize)
{
	_stl->meshes.push_back(K2Mesh());
	K2Mesh& block(*_stl->meshes.rbegin());

	block.index = file->ReadInt();
	block.mode = file->ReadInt();
	block.numVertices = file->ReadInt();
	file->ReadType<SVec3>(block.bbMin);
	file->ReadType<SVec3>(block.bbMax);
	block.bone = file->ReadInt();

	block.nameSize = file->ReadByte();
	block.name2Size = file->ReadByte();
	file->ReadArray(block.name, block.nameSize + 1);

	return true;
}


//===================
// ImportK2Model::ParseVertices
//===================
bool			ImportK2Model::ParseVertices(GrDriver& driver, EFile* file, uint blockSize)
{
	uint meshIndex(file->ReadInt());
	E_VERIFY(meshIndex >= 0 && meshIndex < _stl->meshes.size(), return false);
	K2Mesh& k2mesh(_stl->meshes[meshIndex]);

	E_DELETE_ARRAY("k2mesh", k2mesh.positions);
	k2mesh.positions = E_NEW_ARRAY("k2mesh", SVec3, k2mesh.numVertices);
	float* pos = (float*)k2mesh.positions;

	for (int i = 0; i < k2mesh.numVertices; ++i, pos += 3)
	{
		pos[0] = -file->ReadNum();
		pos[1] = -file->ReadNum();
		pos[2] = file->ReadNum();

		ESwap(pos[1], pos[2]);
	}

	return true;
}


//===================
// ImportK2Model::ParseTexcoords
//===================
bool			ImportK2Model::ParseTexcoords(GrDriver& driver, EFile* file, uint blockSize)
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
		uv[0] = file->ReadNum();
		uv[1] = file->ReadNum();
	}

	return true;
}


//===================
// ImportK2Model::ParseTriangles
//===================
bool			ImportK2Model::ParseTriangles(GrDriver& driver, EFile* file, uint blockSize)
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
			file->ReadType<TriIdx>(indices[0]);
			file->ReadType<TriIdx>(indices[1]);
			file->ReadType<TriIdx>(indices[2]);
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

	while (file->GetPos() + 4 + sizeof(int) < file->GetFileSize())
	{
		char blockName[5];
		for (int i = 0; i < 4; ++i)
			blockName[i] = (char)file->ReadByte();
		blockName[4] = 0;

		int blockSize = file->ReadInt();

		uint filePos(file->GetPos());

		if (strncmp(blockName, "mesh", 4) == 0)
			ParseMesh(_driver, _file, (uint)blockSize);
		else if (strncmp(blockName, "vrts", 4) == 0)
			ParseVertices(_driver, _file, (uint)blockSize);
		else if (strncmp(blockName, "texc", 4) == 0)
			ParseTexcoords(_driver, _file, (uint)blockSize);
		else if (strncmp(blockName, "face", 4) == 0)
			ParseTriangles(_driver, _file, (uint)blockSize);

		file->Seek(filePos + blockSize);
	}
	file->Close();

	// finished reading, build all the meshes.
	for (size_t i = 0; i < _stl->meshes.size(); ++i)
	{
		K2Mesh& k2mesh(_stl->meshes[i]);

		GrMesh* mesh(_driver.CreateMesh(_T("k2mesh"),
			k2mesh.positions, k2mesh.texcoords, k2mesh.numVertices,
			k2mesh.triangles, k2mesh.numTriangles));
		if (mesh != NULL)
		{
			GrModelNode* modelNode(E_NEW("k2mesh", GrModelNode)(StringToWString(k2mesh.name)));
			modelNode->SetMesh(mesh);
			modelNode->AddMeshRange(GrModelNode::SMeshRange(0, k2mesh.numTriangles, _nullMat));
			_model->GetRoot().AddChildModelNode(modelNode);
		}
	}

	return true;
}

