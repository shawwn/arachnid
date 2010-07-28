//========================================================================
//	file:		import_k2_map.h
//	author:		Shawn Presser 
//	date:		7/28/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "m_vec3.h"
//========================================================================

//========================================================================
// Declarations
//========================================================================
class GrDriver;
class MPlane;
struct ImportK2Map_impl;
//========================================================================

//========================================================================
// Constants
//========================================================================
#define TILE_SPACE		0
#define GRID_SPACE		1
#define NUM_SPACES		2

#define TRIANGLE_LEFT	0
#define TRIANGLE_RIGHT	1
#define NUM_SPLITS		2
//========================================================================

//========================================================================
// ImportK2Map
//========================================================================
class ENGINE_API ImportK2Map
{
private:
	ImportK2Map_impl*	_impl;

	GrDriver&			_driver;
	wstring				_name;

	int					_dim;
	float				_scale;

	int					_width[NUM_SPACES];
	int					_height[NUM_SPACES];
	int					_area[NUM_SPACES];

	float*				_grid;
	byte*				_cliffs;
	byte*				_splits;
	MPlane*				_tileNormals[NUM_SPLITS];
	SVec3*				_normals;

	void				SetSize(int dim);

	bool				IsInBounds(int x, int y, uint space);
	int					GetGridIndex(int x, int y);
	int					GetTileIndex(int x, int y);

	float				GetGridPoint(int x, int y)		{ return _grid[GetGridIndex(x, y)]; }
	byte				GetTileSplit(int x, int y)		{ return _splits[GetTileIndex(x, y)]; }
	byte				GetCliff(int x, int y)			{ return _cliffs[GetTileIndex(x, y)]; }
	const SVec3&		GetTileNormal(int x, int y, uint split);

	bool				ParseHeightMap();
	bool				ParseCliffs();

	void				CalcSplits();

	void				CalcTileNormal(int x, int y);
	void				CalcTileNormals();

	void				CalcNormal(int x, int y);
	void				CalcNormals();
public:
	ImportK2Map(GrDriver& driver, const wstring& name, int size, float scale);
	~ImportK2Map();

	bool				Read();
};
//========================================================================




