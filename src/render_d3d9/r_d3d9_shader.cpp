//========================================================================
//	file:		r_d3d9_shader.h
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_common.h"
#include "r_d3d9_shader.h"

// engine headers.
#include "../engine/e_filemanager.h"
#include "../engine/e_filemem.h"

// graphics headers.
#include "r_d3d9_shader_constants.h"
//========================================================================

//===================
// RD3D9Shader::RD3D9Shader
//===================
RD3D9Shader::RD3D9Shader()
: _vs(NULL)
, _ps(NULL)
, _vsConstantTable(NULL)
, _psConstantTable(NULL)
{
}


//===================
// RD3D9Shader::~RD3D9Shader
//===================
RD3D9Shader::~RD3D9Shader()
{
	SAFE_RELEASE(_vsConstantTable);
	SAFE_RELEASE(_psConstantTable);
	SAFE_RELEASE(_vs);
	SAFE_RELEASE(_ps);
}


//===================
// RD3D9Shader::Create
//===================
RD3D9Shader*		RD3D9Shader::Create(const wstring& name, wstring& errors)
{
	errors.clear();

	//const char* shaderProfile(D3DXGetVertexShaderProfile(gDevice));

	IDirect3DVertexShader9* vs(NULL);
	ID3DXConstantTable* vsConstants(NULL);
	IDirect3DPixelShader9* ps(NULL);
	ID3DXConstantTable* psConstants(NULL);

	wstring vsProfile(_T("vs_2_0"));
	const char* vsProfileC("vs_2_0");
	const char* vsFunc("VS");

	wstring psProfile(_T("ps_2_0"));
	const char* psProfileC("ps_2_0");
	const char* psFunc("PS");

	const wstring vsPath(_TP("%cgame/shaders/%s/%s.vsh", FILE_PATH_ROOT_DIR, vsProfile.c_str(), name.c_str()));
	const wstring psPath(_TP("%cgame/shaders/%s/%s.psh", FILE_PATH_ROOT_DIR, psProfile.c_str(), name.c_str()));

	DWORD flags = 0;

	// load vertex shader.
	if (gFileManager->Exists(vsPath))
	{
		wstring path(gFileManager->GetAbsolutePath(vsPath));

		// compile.
		ID3DXBuffer* bytecode(NULL);
		ID3DXBuffer* errBuf(NULL);
		HRESULT hr = D3DXCompileShaderFromFile(path.c_str(),
			NULL, NULL, vsFunc, vsProfileC, flags, &bytecode, &errBuf, NULL);

		// report errors.
		if (errBuf != NULL)
		{
			errors = ToWideString((const char*)errBuf->GetBufferPointer());
			E_ASSERT(errors.empty());
			errBuf->Release();
			E_ASSERT(bytecode == NULL);
			return NULL;
		}

		E_VERIFY(bytecode != NULL, return NULL);

		// get constant table.
		hr = D3DXGetShaderConstantTable((DWORD*)bytecode->GetBufferPointer(), &vsConstants);
		if (FAILED(hr))
		{
			errors = _T("D3DXGetShaderConstantTable failed.");
			E_ASSERT(errors.empty());
			bytecode->Release();
			return NULL;
		}

		// create the actual shader.
		hr = gDevice->CreateVertexShader((DWORD*)bytecode->GetBufferPointer(), &vs);
		bytecode->Release();

		if (FAILED(hr))
		{
			errors = _T("CreateVertexShader failed.");
			E_ASSERT(errors.empty());
			return NULL;
		}
	}

	// load pixel shader.
	if (gFileManager->Exists(psPath))
	{
		wstring path(gFileManager->GetAbsolutePath(psPath));

		// compile.
		ID3DXBuffer* bytecode(NULL);
		ID3DXBuffer* errBuf(NULL);
		HRESULT hr = D3DXCompileShaderFromFile(path.c_str(),
			NULL, NULL, psFunc, psProfileC, flags, &bytecode, &errBuf, NULL);

		// report errors.
		if (errBuf != NULL)
		{
			errors = ToWideString((const char*)errBuf->GetBufferPointer());
			E_ASSERT(errors.empty());
			errBuf->Release();
			E_ASSERT(bytecode == NULL);
			return NULL;
		}

		E_VERIFY(bytecode != NULL, return NULL);

		// get constant table.
		hr = D3DXGetShaderConstantTable((DWORD*)bytecode->GetBufferPointer(), &psConstants);
		if (FAILED(hr))
		{
			errors = _T("D3DXGetShaderConstantTable failed.");
			E_ASSERT(errors.empty());
			bytecode->Release();
			return NULL;
		}

		// create the actual shader.
		hr = gDevice->CreatePixelShader((DWORD*)bytecode->GetBufferPointer(), &ps);
		bytecode->Release();

		if (FAILED(hr))
		{
			errors = _T("CreatePixelShader failed.");
			E_ASSERT(errors.empty());
			return NULL;
		}
	}

	RD3D9Shader* shader(E_NEW("d3d9_shader", RD3D9Shader));
	shader->_vs = vs;
	shader->_vsConstantTable = vsConstants;
	shader->_ps = ps;
	shader->_psConstantTable = psConstants;

	// get the vertex constants.
	for (uint i = 0; i < VC_COUNT; ++i)
	{
		const char* constantName(gVertexShaderConstantNames[i]);
		D3DXHANDLE handle = shader->_vsConstantTable->GetConstantByName(NULL, constantName);
		if (handle != NULL)
		{
			UINT count(1);
			D3DXCONSTANT_DESC desc;
			HRESULT hr = shader->_vsConstantTable->GetConstantDesc(handle, &desc, &count);
			E_VERIFY(SUCCEEDED(hr), continue);

			Constant constant;
			constant.handle = handle;
			constant.desc = desc;
			constant.constant = &gShaderConstants->GetVertexConstant((EVertexShaderConstant)i);
			shader->_vsConstants.push_back(constant);
		}
	}

	// get the pixel constants.
	for (uint i = 0; i < PC_COUNT; ++i)
	{
		const char* constantName(gPixelShaderConstantNames[i]);
		D3DXHANDLE handle = shader->_psConstantTable->GetConstantByName(NULL, constantName);
		if (handle != NULL)
		{
			UINT count(1);
			D3DXCONSTANT_DESC desc;
			HRESULT hr = shader->_psConstantTable->GetConstantDesc(handle, &desc, &count);
			E_VERIFY(SUCCEEDED(hr), continue);

			Constant constant;
			constant.handle = handle;
			constant.desc = desc;
			constant.constant = &gShaderConstants->GetPixelConstant((EPixelShaderConstant)i);
			constant.samplerIdx = (DWORD)(-1);

			if (desc.RegisterSet == D3DXRS_SAMPLER)
				constant.samplerIdx = shader->_psConstantTable->GetSamplerIndex(handle);

			shader->_psConstants.push_back(constant);
		}
	}

	return shader;
}


//===================
// RD3D9Shader::Apply
//===================
void					RD3D9Shader::Apply()
{
	if (_vs != NULL)
	{
		for (size_t i = 0; i < _vsConstants.size(); ++i)
		{
			Constant& constant(_vsConstants[i]);
			switch (constant.desc.Class)
			{
			case D3DXPC_MATRIX_COLUMNS:
				_vsConstantTable->SetMatrix(gDevice, constant.handle, &constant.constant->GetMatrix());
				break;

			case D3DXPC_MATRIX_ROWS:
				_vsConstantTable->SetMatrixTranspose(gDevice, constant.handle, &constant.constant->GetMatrix());
				break;

			case D3DXPC_VECTOR:
				_vsConstantTable->SetVector(gDevice, constant.handle, &constant.constant->GetVector());
				break;

			default:
				E_ASSERT(false);
				break;
			}
		}
	}

	if (_ps != NULL)
	{
		for (size_t i = 0; i < _psConstants.size(); ++i)
		{
			Constant& constant(_psConstants[i]);

			if (constant.desc.RegisterSet == D3DXRS_SAMPLER)
			{
				gDevice->SetSamplerState(constant.samplerIdx, D3DSAMP_ADDRESSU, constant.constant->GetAddressU());
				gDevice->SetSamplerState(constant.samplerIdx, D3DSAMP_ADDRESSV, constant.constant->GetAddressV());
				gDevice->SetTexture(constant.samplerIdx, constant.constant->GetTexture());
			}
			else
			{
				switch (constant.desc.Class)
				{
				case D3DXPC_MATRIX_COLUMNS:
					_psConstantTable->SetMatrix(gDevice, constant.handle, &constant.constant->GetMatrix());
					break;

				case D3DXPC_MATRIX_ROWS:
					_psConstantTable->SetMatrixTranspose(gDevice, constant.handle, &constant.constant->GetMatrix());
					break;

				case D3DXPC_VECTOR:
					_psConstantTable->SetVector(gDevice, constant.handle, &constant.constant->GetVector());
					break;

				default:
					E_ASSERT(false);
					break;
				}
			}
		}
	}

	gDevice->SetVertexShader(_vs);
	gDevice->SetPixelShader(_ps);

}

