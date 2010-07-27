//========================================================================
//	file:		r_d3d9_shader.h
//	author:		Shawn Presser 
//	date:		7/27/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_shader_constants.h"
//========================================================================

//========================================================================
// RD3D9Shader
//========================================================================
class RD3D9Shader
{
	struct Constant
	{
		D3DXHANDLE				handle;
		D3DXCONSTANT_DESC		desc;
		RD3D9ShaderConstant*	constant;
	};

private:
	vector<Constant>			_vsConstants;
	vector<Constant>			_psConstants;

	IDirect3DVertexShader9*		_vs;
	ID3DXConstantTable*			_vsConstantTable;

	IDirect3DPixelShader9*		_ps;
	ID3DXConstantTable*			_psConstantTable;

	RD3D9Shader();
	~RD3D9Shader();

public:
	static RD3D9Shader*		Create(const wstring& name, wstring& errors);
	void					Apply();
};
//========================================================================

