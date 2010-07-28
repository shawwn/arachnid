//========================================================================
//	file:		import_k2_map.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "import_k2_map.h"

// engine headers.
#include "e_filemanager.h"
#include "e_filemem.h"

// graphics headers.
#include "gr_driver.h"
#include "gr_model.h"
#include "gr_model_node.h"
#include "gr_skin.h"

// math headers.
#include "m_vec3.h"
#include "m_plane.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define SPLIT_NEG			0
#define SPLIT_POS			1
//========================================================================

//===================
// ImportK2Map::SetSize
//===================
void			ImportK2Map::SetSize(int dim)
{
	_dim = CLAMP(dim, 1, 10);

	int width = (1 << _dim);
	int height = (1 << _dim);

	_width[TILE_SPACE] = width;
	_height[TILE_SPACE] = height;

	_width[GRID_SPACE] = width + 1;
	_height[GRID_SPACE] = height + 1;

	_area[TILE_SPACE] = _width[TILE_SPACE] * _height[TILE_SPACE];
	_area[GRID_SPACE] = _width[GRID_SPACE] * _height[GRID_SPACE];
}


//===================
// ImportK2Map::IsInBounds
//===================
bool			ImportK2Map::IsInBounds(int x, int y, uint space)
{
	E_ASSERT(space < NUM_SPACES);

	if (x < 0 || x >= _width[space] ||
		y < 0 || y >= _height[space])
	{
		return false;
	}

	return true;
}


//===================
// ImportK2Map::GetGridIndex
//===================
int				ImportK2Map::GetGridIndex(int x, int y)
{
	if (x < 0 || x >= _width[GRID_SPACE] ||
		y < 0 || y >= _height[GRID_SPACE])
	{
		E_WARN("ImportK2Map::GetGridIndex", "out of bounds coordinate");
		x = CLAMP(0, _width[GRID_SPACE] - 1, x);
		y = CLAMP(0, _height[GRID_SPACE] - 1, y);
	}

	return y*_width[GRID_SPACE] + x;
}


//===================
// ImportK2Map::GetTileIndex
//===================
int				ImportK2Map::GetTileIndex(int x, int y)
{
	if (x < 0 || x >= _width[TILE_SPACE] ||
		y < 0 || y >= _height[TILE_SPACE])
	{
		E_WARN("ImportK2Map::GetTileIndex", "out of bounds coordinate");
		x = CLAMP(0, _width[TILE_SPACE] - 1, x);
		y = CLAMP(0, _height[TILE_SPACE] - 1, y);
	}

	return y*_width[TILE_SPACE] + x;
}


//===================
// ImportK2Map::GetTileNormal
//===================
const SVec3&	ImportK2Map::GetTileNormal(int x, int y, uint split)
{
	E_VERIFY(split < NUM_SPLITS,
		split = CLAMP(0, NUM_SPLITS - 1, split));

	return *(SVec3*)_tileNormals[split][GetTileIndex(x, y)].Norm().Get();
}


//===================
// ImportK2Map::ParseHeightMap
//===================
bool			ImportK2Map::ParseHeightMap()
{
	wstring path(_TP("%cgame/maps/%s/heightmap", FILE_PATH_ROOT_DIR, _name.c_str()));
	EFileMem* file = (EFileMem*)gFileManager->GetFile(path, FILE_READ | FILE_BINARY | FILE_MEMORY);
	if (file == NULL)
		return false;

	// width & height
	int width = file->ReadInt();
	int height = file->ReadInt();

	// versioning
	bool newformat = (width < 0);
	width = ABS(width);

	E_VERIFY(width == _width[GRID_SPACE] && height == _height[GRID_SPACE], return false);

	int area = width * height;
	E_ASSERT(area > 0);

	// verts
	E_ASSERT(_grid == NULL);
	_grid = (float*)MemAlloc(sizeof(float) * area);
	if (newformat)
	{
		for (int i = 0; i < area; ++i)
			_grid[i] = file->ReadByte() / 256.0f;
		for (int i = 0; i < area; ++i)
			_grid[i] += file->ReadByte();
		for (int i = 0; i < area; ++i)
		{
			_grid[i] += file->ReadByte() * 256.0f;
			_grid[i] -= 32768.0f;
		}
	}
	else
	{
		file->ReadArray(_grid, area);
	}

	// done
	E_DELETE("import_k2_map", file);

	return true;
}


//===================
// ImportK2Map::ParseCliffs
//===================
bool			ImportK2Map::ParseCliffs()
{
	wstring path(_TP("%cgame/maps/%s/tilecliffmap", FILE_PATH_ROOT_DIR, _name.c_str()));
	EFileMem* file = (EFileMem*)gFileManager->GetFile(path, FILE_READ | FILE_BINARY | FILE_MEMORY);
	if (file == NULL)
		return false;

	// width & height
	int width = file->ReadInt();
	int height = file->ReadInt();
	E_VERIFY(width == _width[TILE_SPACE] && height == _height[TILE_SPACE], return false);

	int area = width * height;
	E_ASSERT(area > 0);

	// cliffs
	E_ASSERT(_cliffs == NULL);
	_cliffs = (byte*)MemAlloc(sizeof(byte) * area);
	file->ReadArray(_cliffs, area);

	// done
	E_DELETE("import_k2_map", file);

	return true;
}


//===================
// ImportK2Map::CalcSplits
//===================
void			ImportK2Map::CalcSplits()
{
	MemFree(_splits);
	_splits = (byte*)MemAlloc(sizeof(byte) * _area[TILE_SPACE]);

	for (int y = 0; y < _height[TILE_SPACE]; ++y)
	{
		for (int x = 0; x < _width[TILE_SPACE]; ++x)
		{
			float h1(GetGridPoint(x, y));
			float h2(GetGridPoint(x, y + 1));
			float h3(GetGridPoint(x + 1, y));
			float h4(GetGridPoint(x + 1, y + 1));

			if (ABS(h1 - h4) == ABS(h2 - h3))
				_splits[GetTileIndex(x, y)] = (x % 2 + y % 2) % 2;
			else if (ABS(h1 - h4) < ABS(h2 - h3))
				_splits[GetTileIndex(x, y)] = SPLIT_POS;
			else
				_splits[GetTileIndex(x, y)] = SPLIT_NEG;
		}
	}
}


//===================
// ImportK2Map::CalcTileNormal
//===================
void			ImportK2Map::CalcTileNormal(int x, int y)
{
	float h1(GetGridPoint(x, y));
	float h2(GetGridPoint(x, y + 1));
	float h3(GetGridPoint(x + 1, y));
	float h4(GetGridPoint(x + 1, y + 1));

	int index(GetTileIndex(x, y));

	MVec3 a(x * _scale, y * _scale, h1);
	MVec3 b(x * _scale, (y + 1) * _scale, h2);
	MVec3 c((x + 1) * _scale, y * _scale, h3);
	MVec3 d((x + 1) * _scale, (y + 1) * _scale, h4);

	if (GetTileSplit(x, y) == SPLIT_NEG)
	{
		_tileNormals[TRIANGLE_LEFT][index] = MPlane::FromPoints(c, b, a);
		_tileNormals[TRIANGLE_RIGHT][index] = MPlane::FromPoints(d, b, c);
	}
	else
	{
		_tileNormals[TRIANGLE_LEFT][index] = MPlane::FromPoints(a, d, b);
		_tileNormals[TRIANGLE_RIGHT][index] = MPlane::FromPoints(a, c, d);
	}
}


//===================
// ImportK2Map::CalcTileNormals
//===================
void			ImportK2Map::CalcTileNormals()
{
	E_DELETE_ARRAY("import_k2_map", _tileNormals[TRIANGLE_LEFT]);
	E_DELETE_ARRAY("import_kimport_k2_map", _tileNormals[TRIANGLE_RIGHT]);

	_tileNormals[TRIANGLE_LEFT] = E_NEW_ARRAY("import_k2_map", MPlane, _area[TILE_SPACE]);
	_tileNormals[TRIANGLE_RIGHT] = E_NEW_ARRAY("import_k2_map", MPlane, _area[TILE_SPACE]);

	for (int y = 0; y < _width[TILE_SPACE]; ++y)
		for (int x = 0; x < _height[TILE_SPACE]; ++x)
			CalcTileNormal(x, y);
}


//===================
// ImportK2Map::CalcNormal
//===================
void			ImportK2Map::CalcNormal(int x, int y)
{
	E_VERIFY(IsInBounds(x, y, GRID_SPACE), return);

	SVec3&	normal(_normals[GetGridIndex(x, y)]);
	normal.SetZero();

	if (x > 0 && y > 0)
	{
		if (!GetCliff(x - 1, y - 1))
		{
			if (GetTileSplit(x - 1, y - 1) == SPLIT_NEG)
			{
				normal += GetTileNormal(x - 1, y - 1, TRIANGLE_RIGHT);
			}
			else
			{
				normal += 0.5f * GetTileNormal(x - 1, y - 1, TRIANGLE_LEFT);
				normal += 0.5f * GetTileNormal(x - 1, y - 1, TRIANGLE_RIGHT);
			}
		}
	}
}


//===================
// ImportK2Map::CalcNormals
//===================
void			ImportK2Map::CalcNormals()
{
	MemFree(_normals);
	_normals = (SVec3*)MemAlloc(sizeof(SVec3) * _area[GRID_SPACE]);

	for (int y = 0; y < _height[GRID_SPACE]; ++y)
		for (int x = 0; x < _width[GRID_SPACE]; ++x)
			CalcNormal(x, y);
}


//===================
// ImportK2Map::ImportK2Map
//===================
ImportK2Map::ImportK2Map(GrDriver& driver, const wstring& name, int size, float scale)
: _driver(driver)
, _name(name)
, _scale(scale)
, _grid(NULL)
, _cliffs(NULL)
, _splits(NULL)
, _normals(NULL)
{
	_tileNormals[TRIANGLE_LEFT] = NULL;
	_tileNormals[TRIANGLE_RIGHT] = NULL;
	SetSize(size);
}


//===================
// ImportK2Map::~ImportK2Map
//===================
ImportK2Map::~ImportK2Map()
{
	E_DELETE_ARRAY("import_k2_map", _tileNormals[TRIANGLE_LEFT]);
	E_DELETE_ARRAY("import_kimport_k2_map", _tileNormals[TRIANGLE_RIGHT]);
	MemFree(_grid);
	MemFree(_cliffs);
	MemFree(_splits);
	MemFree(_normals);
}


//===================
// ImportK2Map::Read
//===================
bool			ImportK2Map::Read()
{
	E_VERIFY(ParseHeightMap(), return false);
	E_VERIFY(ParseCliffs(), return false);

	CalcSplits();
	CalcTileNormals();
	CalcNormals();

	return true;
}

