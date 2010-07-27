//========================================================================
//	file:		r_d3d9_shader_constants.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_common.h"
#include "r_d3d9_shader_constants.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
RD3D9ShaderConstants*		gShaderConstants;

const char*			gVertexShaderConstantNames[VC_COUNT] =
{
	"_worldViewProj"
};
const char*			gPixelShaderConstantNames[PC_COUNT] =
{
	"_diffuseColor",
	"_diffuse"
};
//========================================================================

//========================================================================
// RD3D9ShaderConstant
//========================================================================

//===================
// RD3D9ShaderConstant::RD3D9ShaderConstant
//===================
RD3D9ShaderConstant::RD3D9ShaderConstant()
: _name("invalid")
, _tex(NULL)
{
}


//===================
// RD3D9ShaderConstant::Reset
//===================
void						RD3D9ShaderConstant::Reset(const char* name)
{
	_name = name;
	_vec = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	_tex = NULL;
	_sampler = 0;
	D3DXMatrixIdentity(&_mat);
}
//========================================================================

//========================================================================
// RD3D9ShaderConstants
//========================================================================

//===================
// RD3D9ShaderConstants::RD3D9ShaderConstants
//===================
RD3D9ShaderConstants::RD3D9ShaderConstants()
{
	for (uint i = 0; i < VC_COUNT; ++i)
		_vertexConstants[i].Reset(gVertexShaderConstantNames[i]);

	for (uint i = 0; i < PC_COUNT; ++i)
		_pixelConstants[i].Reset(gPixelShaderConstantNames[i]);

	_pixelConstants[PC_DIFFUSE].SetSampler(0);

	gShaderConstants = this;
}


//===================
// RD3D9ShaderConstants::~gRD3D9ShaderConstants
//===================
RD3D9ShaderConstants::~RD3D9ShaderConstants()
{
	gShaderConstants = NULL;
}


//===================
// RD3D9ShaderConstants::GetVertexConstant
//===================
RD3D9ShaderConstant&			RD3D9ShaderConstants::GetVertexConstant(EVertexShaderConstant which)
{
	E_ASSERT(which >= 0 && which < VC_COUNT);
	return _vertexConstants[which];
}


//===================
// RD3D9ShaderConstants::GetPixelConstant
//===================
RD3D9ShaderConstant&			RD3D9ShaderConstants::GetPixelConstant(EPixelShaderConstant which)
{
	E_ASSERT(which >= 0 && which < PC_COUNT);
	return _pixelConstants[which];
}
//========================================================================
