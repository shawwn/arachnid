//========================================================================
//	file:		r_d3d9_shader_constants.h
//	author:		Shawn Presser 
//	date:		7/27/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
enum EShaderConstantType
{
	SCT_MATRIX,
	SCT_VECTOR,
	SCT_VALUE
};
enum EVertexShaderConstant
{
	VC_WORLD_VIEW_PROJ,
	VC_COUNT
};
enum EPixelShaderConstant
{
	PC_DIFFUSE_COLOR,
	PC_DIFFUSE,
	PC_COUNT
};
extern const char*		gVertexShaderConstantNames[VC_COUNT];
extern const char*		gPixelShaderConstantNames[PC_COUNT];
//========================================================================

//========================================================================
// RD3D9ShaderConstant
//========================================================================
class RD3D9ShaderConstant
{
private:
	D3DXVECTOR4					_vec;
	D3DXMATRIX					_mat;
	IDirect3DBaseTexture9*		_tex;
	const char*					_name;
	DWORD						_sampler;

public:
	RD3D9ShaderConstant();

	void						Reset(const char* name);

	void						SetMatrix(const D3DXMATRIX& mat)		{ _mat = mat; }
	void						SetVector(const D3DXVECTOR4& vec)		{ _vec = vec; }
	void						SetVector(const D3DXVECTOR3& vec)		{ _vec = D3DXVECTOR4(vec.x, vec.y, vec.z, 1.0f); }
	void						SetValue(float val)						{ _vec = D3DXVECTOR4(val, val, val, val); }
	void						SetTexture(IDirect3DBaseTexture9* tex)	{ _tex = tex; }
	void						SetSampler(DWORD sampler)				{ _sampler = sampler; }

	const D3DXMATRIX&			GetMatrix() const					{ return _mat; }
	const D3DXVECTOR4&			GetVector() const					{ return _vec; }
	IDirect3DBaseTexture9*		GetTexture() const					{ return _tex; }
	DWORD						GetSampler() const					{ return _sampler; }
};
//========================================================================

//========================================================================
// RD3D9ShaderConstants
//========================================================================
class RD3D9ShaderConstants
{
private:
	RD3D9ShaderConstant				_vertexConstants[VC_COUNT];
	RD3D9ShaderConstant				_pixelConstants[PC_COUNT];

	void							InitConstants();

public:
	RD3D9ShaderConstants();
	~RD3D9ShaderConstants();

	RD3D9ShaderConstant&			GetVertexConstant(EVertexShaderConstant which);
	RD3D9ShaderConstant&			GetPixelConstant(EPixelShaderConstant which);
};
extern RD3D9ShaderConstants*		gShaderConstants;
//========================================================================