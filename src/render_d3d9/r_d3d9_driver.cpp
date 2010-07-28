//========================================================================
//	file:		r_d3d9_driver.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_common.h"
#include "r_d3d9_driver.h"

// engine headers.
#include "../engine/e_system.h"
#include "../engine/e_engine.h"
#include "../engine/e_filemanager.h"

// graphics headers.
#include "../engine/gr_driver.h"
#include "../engine/gr_scene.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_texture.h"
#include "../engine/gr_material.h"
#include "../engine/gr_light.h"
#include "../engine/gr_skin.h"

// d3d9 headers.
#include "r_d3d9_shader_constants.h"
#include "r_d3d9_shader.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define USE_D3D9EX		1
#define RENDER_NORMALS	0
//========================================================================

//========================================================================
// Declarations
//========================================================================
static LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM	wParam,
						 LPARAM	lParam);
//========================================================================

//========================================================================
// Globals
//========================================================================
IDirect3DDevice9*	gDevice;
D3D9Driver*			gDriver;
D3D9Driver_impl*	gDriver_impl;
IDirect3DTexture9*	gNullTex;
D3DPOOL				gManagedPool(D3DPOOL_MANAGED);
DWORD				gManagedUsage(0);

D3DXMATRIX			gWorld;
D3DXMATRIX			gView;
D3DXMATRIX			gProj;

RD3D9Shader*		gShaderSimple;
RD3D9Shader*		gShaderColored;
RD3D9Shader*		gSolidAmbient;
RD3D9Shader*		gSolidLit;

RD3D9Shader*		gCurShader;
//========================================================================

//========================================================================
// Vertex Declarations
//========================================================================
IDirect3DVertexDeclaration9*	_meshDecl;
IDirect3DVertexDeclaration9*	_coloredDecl;
//========================================================================

//========================================================================
// Dynamic Textures
//========================================================================
IDirect3DTexture9*	_lightFalloffTex;
//========================================================================

//========================================================================
// Functions
//========================================================================
static void			XFormToD3DXMat(D3DXMATRIX& mat, const MTransform& xform)
{
	const MMat33& rot(xform.GetRot());
	const MVec3& trans(xform.GetTrans());

	mat._11 = rot(0, 0);
	mat._21 = rot(1, 0);
	mat._31 = rot(2, 0);
	mat._41 = trans.X();

	mat._12 = rot(0, 1);
	mat._22 = rot(1, 1);
	mat._32 = rot(2, 1);
	mat._42 = trans.Y();

	mat._13 = rot(0, 2);
	mat._23 = rot(1, 2);
	mat._33 = rot(2, 2);
	mat._43 = trans.Z();

	mat._14 = 0.0f;
	mat._24 = 0.0f;
	mat._34 = 0.0f;
	mat._44 = 1.0f;
}


static D3DXVECTOR4	SVec4ToD3DXVec(const SVec4& v)
{
	return D3DXVECTOR4(v.X(), v.Y(), v.Z(), v.W());
}
//========================================================================

//========================================================================
// D3D9Vertex
//========================================================================
struct D3D9Vertex
{
	SVec3			pos;

	/*
	union
	{
		byte			bgra[4];
		uint			color;
	};
	*/

	SVec3			normal;

	SVec2			uv;
};
//========================================================================

//========================================================================
// D3D9UntexturedVertex
//========================================================================
struct D3D9UntexturedVertex
{
	SVec3			pos;

	union
	{
		byte			bgra[4];
		uint			color;
	};
};
//========================================================================

//========================================================================
// D3D9MeshData
//========================================================================
struct D3D9MeshData
{
	IDirect3DVertexBuffer9*		vbPosNormals;
	IDirect3DVertexBuffer9*		vbTexcoords;
	IDirect3DIndexBuffer9*		ib;

	D3D9MeshData()
		: vbPosNormals(NULL)
		, vbTexcoords(NULL)
		, ib(NULL)
	{
	}

	bool				Update(GrSkin* skin)
	{
		uint numVerts(skin->GetNumVerts());

		// for now, all meshes must have position, tangent, binormal, normal, and texcoord data.
		uint channels(skin->GetMeshChannels());
		E_VERIFY((channels & MESH_TANGENTS), return false);
		E_VERIFY((channels & MESH_BINORMALS), return false);
		E_VERIFY((channels & MESH_NORMALS), return false);
		SVec3* positions(skin->GetPositions());
		SVec3* tangents(skin->GetTangents());
		SVec3* binormals(skin->GetBinormals());
		SVec3* normals(skin->GetNormals());

		HRESULT hr;

		// fill the pos-normals buffer.
		SVec3* vbPosNormalsPtr(NULL);
		hr = vbPosNormals->Lock(0, 0, (void **)&vbPosNormalsPtr, 0);
		E_VERIFY(SUCCEEDED(hr), return false);
		for (uint i = 0; i < numVerts; ++i)
		{
			*vbPosNormalsPtr++ = positions[i];
			*vbPosNormalsPtr++ = tangents[i];
			*vbPosNormalsPtr++ = binormals[i];
			*vbPosNormalsPtr++ = normals[i];
		}
		vbPosNormals->Unlock();

		return true;
	}
};

//========================================================================
// D3D9Driver_impl
//========================================================================
struct D3D9Driver_impl
{
	HINSTANCE			hInstance;
	DWORD				dwStyle;
	HWND				hWnd;
	HDC					hDC;
	IDirect3D9*			d3d9;
	IDirect3DDevice9*	device;
	bool				done;

	D3D9Driver_impl()
		: hInstance(NULL)
		, dwStyle(0)
		, hWnd(NULL)
		, hDC(NULL)
		, d3d9(NULL)
		, device(NULL)
		, done(false)
	{
	}

	static IDirect3D9*		StartupD3D9()
	{
#if USE_D3D9EX
		E_ASSERT(gSystem != NULL);
		void* libHandle(gSystem->LoadLib(_T("d3d9")));
		if (libHandle != NULL)
		{
			typedef HRESULT (WINAPI *Direct3DCreate9ExFunc)(UINT SDKVersion, IDirect3D9Ex **d3d9ex);
			Direct3DCreate9ExFunc pDirect3DCreate9Ex(NULL);

			pDirect3DCreate9Ex = (Direct3DCreate9ExFunc)gSystem->GetLibFunction(libHandle, _T("Direct3DCreate9Ex"));
			if (pDirect3DCreate9Ex != NULL)
			{
				IDirect3D9Ex* pOut(NULL);
				HRESULT hr = pDirect3DCreate9Ex(D3D_SDK_VERSION, &pOut);
				E_VERIFY(SUCCEEDED(hr), return NULL);

				gManagedPool = D3DPOOL_DEFAULT;
				gManagedUsage = D3DUSAGE_DYNAMIC;
				return pOut;
			}
		}
#endif

		return Direct3DCreate9(D3D_SDK_VERSION);
	}
};
//========================================================================


//===================
// D3D9Driver::SetupVertexDecls
//===================
bool		D3D9Driver::SetupVertexDecls()
{
	HRESULT hr;

	/*
    WORD    Stream;     // Stream index
    WORD    Offset;     // Offset in the stream in bytes
    BYTE    Type;       // Data type
    BYTE    Method;     // Processing method
    BYTE    Usage;      // Semantics
    BYTE    UsageIndex; // Semantic index
	*/

	D3DVERTEXELEMENT9 meshElems[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, 0, D3DDECLUSAGE_POSITION, 0 }, // position 
		{ 0, 12, D3DDECLTYPE_FLOAT3, 0, D3DDECLUSAGE_TANGENT,  0 }, // tangent
		{ 0, 24, D3DDECLTYPE_FLOAT3, 0, D3DDECLUSAGE_BINORMAL, 0 }, // binormal
		{ 0, 36, D3DDECLTYPE_FLOAT3, 0, D3DDECLUSAGE_NORMAL,   0 }, // normal
		{ 1,  0, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_TEXCOORD, 0 }, // texcoord
		D3DDECL_END()
	};
	hr = gDevice->CreateVertexDeclaration(meshElems, &_meshDecl);
	E_VERIFY(SUCCEEDED(hr), return false);

	D3DVERTEXELEMENT9 coloredElems[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3,   0, D3DDECLUSAGE_POSITION, 0 }, // position 
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR,  0 },
		D3DDECL_END()
	};
	hr = gDevice->CreateVertexDeclaration(coloredElems, &_coloredDecl);
	E_VERIFY(SUCCEEDED(hr), return false);

	return true;
}


//===================
// D3D9Driver::SetupTextures
//===================
bool		D3D9Driver::SetupTextures()
{
	E_VERIFY(SUCCEEDED(gDevice->CreateTexture(
		LIGHT_FALLOFF_SAMPLES, 1, 
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_R32F,
		D3DPOOL_DEFAULT,
		&_lightFalloffTex, NULL)),
		return false);
	gShaderConstants->GetPixelConstant(PC_LIGHT_FALLOFF).SetTexture(_lightFalloffTex);
	gShaderConstants->GetPixelConstant(PC_LIGHT_FALLOFF).SetAddress(D3DTADDRESS_CLAMP, D3DTADDRESS_WRAP);

	return true;
}


//===================
// D3D9Driver::D3D9Driver
//===================
D3D9Driver::D3D9Driver(int windowWidth, int windowHeight, const wstring& windowTitle)
: _fatalError(false)
, _winIdx(0)
, _impl(E_NEW("renderer", D3D9Driver_impl))
{
	if (gDriver != NULL)
	{
		_fatalError = true;
		return;
	}
	gDriver = this;
	gDriver_impl = _impl;

	// validate parameters.
	windowWidth = E_MAX(640, windowWidth);
	windowHeight = E_MAX(480, windowHeight);

	// windowed mode.
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	_impl->dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

	RECT rect;
	rect.left = (long)0;
	rect.right = (long)windowWidth;
	rect.top = (long)0;
	rect.bottom = (long)windowHeight;

	_impl->hInstance = GetModuleHandle(NULL);

	// register the window class.
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)&WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _impl->hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("arachnid");

	if (!RegisterClass(&wc))
	{
		_fatalError = true;
		return;
	}

	if (!AdjustWindowRect(&rect, _impl->dwStyle, FALSE))
	{
		_fatalError = true;
		return;
	}

	// create the window.
	_impl->hWnd = CreateWindowEx(
		dwExStyle,
		_T("arachnid"),
		windowTitle.c_str(),
		_impl->dwStyle,
		100, 100,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		_impl->hInstance,
		NULL);

	if (_impl->hWnd == NULL)
	{
		_fatalError = true;
		return;
	}

	_impl->hDC = GetDC(_impl->hWnd);
	if (_impl->hDC == NULL)
	{
		_fatalError = true;
		return;
	}

	// startup D3D9.
	_impl->d3d9 = D3D9Driver_impl::StartupD3D9();
	if (_impl->d3d9== NULL)
	{
		_fatalError = true;
		return;
	}

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
	pp.BackBufferCount			=  1;
	pp.MultiSampleType			= D3DMULTISAMPLE_NONE;
	pp.MultiSampleQuality		= 0;
	pp.SwapEffect				=  D3DSWAPEFFECT_DISCARD;
	pp.hDeviceWindow			= _impl->hWnd;
	pp.Flags					= 0;
	pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;
	pp.BackBufferFormat			= D3DFMT_R5G6B5; // for simplicity we'll hard-code this for now.
	pp.EnableAutoDepthStencil	= TRUE;
	pp.AutoDepthStencilFormat	= D3DFMT_D24S8;
	pp.BackBufferWidth			= windowWidth;
	pp.BackBufferHeight			= windowHeight;
	pp.Windowed					= TRUE;

	// create the device.
	HRESULT hr = _impl->d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		_impl->hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&pp,
		&_impl->device);
	if(FAILED(hr))
	{
		_fatalError = true;
		return;
	}
	gDevice = _impl->device;

	SetMousePos((rect.right - rect.left)/2, (rect.bottom - rect.top)/2);
	ShowWindow(_impl->hWnd, SW_SHOW);

	E_NEW("d3d9", RD3D9ShaderConstants);

	SetupVertexDecls();
	SetupTextures();

	// create a shader.
	wstring shaderErrors;
	gShaderSimple = RD3D9Shader::Create(_T("simple"), shaderErrors);
	gShaderColored = RD3D9Shader::Create(_T("colored"), shaderErrors);
	gSolidAmbient = RD3D9Shader::Create(_T("ambient"), shaderErrors);
	gSolidLit = RD3D9Shader::Create(_T("lit"), shaderErrors);
}


//===================
// D3D9Driver::~D3D9Driver
//===================
D3D9Driver::~D3D9Driver()
{
	E_DELETE("d3d9", gShaderSimple);
	E_DELETE("d3d9", gShaderColored);
	E_DELETE("d3d9", gSolidLit);

	E_DELETE("d3d9", gShaderConstants);

	// shutdown d3d9.
	if (_impl->device != NULL)
		_impl->device->Release();
	if (_impl->d3d9 != NULL)
		_impl->d3d9->Release();

	// release the window's DC.
	if (_impl->hWnd && _impl->hDC)
		ReleaseDC(_impl->hWnd, _impl->hDC);

	// destroy the window.
	if (_impl->hWnd != NULL)
	{
		E_ASSERT(_impl->hInstance != NULL);
		UnregisterClass(_T("arachnid"), _impl->hInstance);
		DestroyWindow(_impl->hWnd);
	}

	E_ASSERT(gDriver != NULL);
	gDevice = NULL;
	gDriver = NULL;
	gDriver_impl = NULL;
	E_DELETE("renderer", _impl);
}


//===================
// D3D9Driver::BeginFrame
//===================
bool		D3D9Driver::BeginFrame()
{
	E_VERIFY(!_fatalError, return false);

	if (_impl->done)
	{
		_fatalError = true;
		return false;
	}

	if (IsWindowActive())
		OnPaint();

	// process window messages.
	if (_impl->hWnd != NULL)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// cursor pos.
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(_impl->hWnd, &pt);
	gEngine->OnMousePos(pt.x, pt.y);

	// set shader constants.
	gShaderConstants->GetPixelConstant(PC_DIFFUSE_COLOR).SetVector(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	return true;
}


//===================
// D3D9Driver::EndFrame
//===================
void		D3D9Driver::EndFrame()
{
	E_VERIFY(!_fatalError, return);
}


//===================
// D3D9Driver::ApplyCamera
//===================
void		D3D9Driver::ApplyCamera(const GrCamera& cam)
{
	MVec3 pos, lookAt, up;
	cam.GetEyeInfo(pos, lookAt, up);

	D3DXMATRIX mat;
	D3DXMatrixLookAtRH(&mat,
		(const D3DXVECTOR3*)pos.Get(),
		(const D3DXVECTOR3*)lookAt.Get(),
		(const D3DXVECTOR3*)up.Get());

	_impl->device->SetTransform(D3DTS_VIEW, &mat);
	gView = mat;
}


//===================
// D3D9Driver::ApplyLight
//===================
void		D3D9Driver::ApplyLight(const GrLight& light)
{
	const MVec3& pos(light.GetPos());
	float radius(light.GetRadius());

	// build the falloff texture.
	E_VERIFY(_lightFalloffTex, return);
	D3DLOCKED_RECT rect;
	E_VERIFY(SUCCEEDED(_lightFalloffTex->LockRect(0, &rect, 0, D3DLOCK_DISCARD)), return);
	MemCpy(rect.pBits, (const void*)light.GetFalloff(), sizeof(float) * LIGHT_FALLOFF_SAMPLES);
	_lightFalloffTex->UnlockRect(0);

	gShaderConstants->GetVertexConstant(VC_LIGHT_POS).SetVector(D3DXVECTOR4(pos.X(), pos.Y(), pos.Z(), 1.0f));
	gShaderConstants->GetVertexConstant(VC_LIGHT_DIR).SetVector(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	gShaderConstants->GetVertexConstant(VC_LIGHT_RADIUS).SetVector(D3DXVECTOR4(radius, radius, radius, radius));

	gShaderConstants->GetPixelConstant(PC_LIGHT_POS).SetVector(D3DXVECTOR4(pos.X(), pos.Y(), pos.Z(), 1.0f));
	gShaderConstants->GetPixelConstant(PC_LIGHT_DIR).SetVector(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	gShaderConstants->GetPixelConstant(PC_LIGHT_RADIUS).SetVector(D3DXVECTOR4(radius, radius, radius, radius));

	gShaderConstants->GetPixelConstant(PC_DIFFUSE_COLOR).SetVector(SVec4ToD3DXVec(light.GetColor()));
}


//===================
// D3D9Driver::ApplyMaterial
//===================
void		D3D9Driver::ApplyMaterial(GrMaterial* mat, EMaterialPass pass)
{
	gCurShader = NULL;

	switch (mat->GetMaterialType())
	{
	case MAT_SOLID:
		{
			if (pass == MAT_PASS_AMBIENT || pass == MAT_PASS_LIT)
			{
				if (pass == MAT_PASS_AMBIENT)
				{
					gShaderConstants->GetPixelConstant(PC_AMBIENT_COLOR).SetVector(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
					gDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
					gDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					gDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
					gDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
					gCurShader = gSolidAmbient;
				}

				if (pass == MAT_PASS_LIT)
				{
					gDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					gDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					gDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
					gDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
					gCurShader = gSolidLit;
				}

				gDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
				gDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
				gDevice->SetVertexDeclaration(_meshDecl);

				// set textures.
				for (uint i = 0; i < MTEX_COUNT; ++i)
				{
					GrTexture* tex(mat->GetTexture((EMaterialTexture)i));

					if (tex != NULL)
						gShaderConstants->GetPixelConstant((EPixelShaderConstant)i).SetTexture((IDirect3DBaseTexture9*)tex->GetUserdata());
					else
						gShaderConstants->GetPixelConstant((EPixelShaderConstant)i).SetTexture(gNullTex);
				}
			}
		}
		break;
	}
}


//===================
// D3D9Driver::DrawMeshRange
//===================
void		D3D9Driver::DrawMeshRange(const MTransform& xform, GrMesh* mesh, uint triStart, uint triCount)
{
	if (gCurShader == NULL)
		return;

	GrSkin* skin(mesh->GetSkin());

	D3D9MeshData* meshData((D3D9MeshData*)mesh->_userdata);
	if (skin->GetDeformed())
	{
		meshData->Update(skin);
		skin->ClearDeformed();
	}

	gDevice->SetStreamSource(0, meshData->vbPosNormals, 0, 4*sizeof(SVec3));
	gDevice->SetStreamSource(1, meshData->vbTexcoords, 0, sizeof(SVec2));
	gDevice->SetIndices(meshData->ib);

	// set transform.
	D3DXMATRIX world;
	XFormToD3DXMat(world, xform);
	gShaderConstants->GetVertexConstant(VC_WORLD_VIEW_PROJ).SetMatrix(world * gView * gProj);

	// set shader.
	gCurShader->Apply();

#if 1
	uint numVerts(skin->GetNumVerts());
	HRESULT hr = gDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 3*triStart, triCount);
	E_ASSERT(SUCCEEDED(hr));
#else
	const SVec3* positions(skin->GetPositions());
	const SVec3* normals(skin->GetNormals());

	const SVec2* texcoords(skin->GetTexcoords());
	const TriIdx* indices(skin->GetIndices());

	for (uint tri = triStart; tri < triStart + triCount; ++tri)
	{
		D3D9Vertex vert[3];

		for (uint corner = 0; corner < 3; ++corner)
		{
			uint idx(indices[3*tri + corner]);
			vert[corner].pos = positions[idx];
			vert[corner].normal = normals ? normals[idx] : SVec3::Zero;
			vert[corner].uv = texcoords[idx];
			vert[corner].pos.RotateTranslate(xform);
			vert[corner].uv.Y() = 1.0f - vert[corner].uv.Y();
		}

		HRESULT hr = gDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, (const void*)vert, sizeof(D3D9Vertex));
		if (FAILED(hr))
		{
			_fatalError = true;
			return;
		}
	}
#endif

	gDevice->SetVertexShader(NULL);
	gDevice->SetPixelShader(NULL);
}


//===================
// D3D9Driver::BeginLines
//===================
void		D3D9Driver::BeginLines()
{
	gDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	gDevice->SetVertexDeclaration(NULL);
	gDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	gDevice->SetPixelShader(NULL);
	gDevice->SetVertexShader(NULL);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	gDevice->SetTransform(D3DTS_WORLD, &mat);
}


//===================
// D3D9Driver::DrawLine
//===================
void		D3D9Driver::DrawLine(const uint& startCol, const SVec3& startPos, const uint& endCol, const SVec3& endPos)
{
	D3D9UntexturedVertex v[2];

	v[0].color = startCol;
	v[1].color = endCol;

	v[0].pos = startPos;
	v[1].pos = endPos;

	gDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, (CONST VOID*)&v, sizeof(D3D9UntexturedVertex));
}


//===================
// D3D9Driver::DrawLines
//===================
void		D3D9Driver::DrawLines(const SColoredVertex* lines, uint count)
{
	gDevice->DrawPrimitiveUP(D3DPT_LINELIST, count, (CONST VOID*)lines, sizeof(SColoredVertex));
}


//===================
// D3D9Driver::SetMousePos
//===================
void		D3D9Driver::SetMousePos(int x, int y)
{
	E_VERIFY(!_fatalError, return);

	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(_impl->hWnd, &pt);

	SetCursorPos(pt.x, pt.y);
}


//===================
// D3D9Driver::CreateMesh
//===================
GrMesh*		D3D9Driver::CreateMesh(const wchar_t* ctx, GrSkin* geometry)
{
	HRESULT hr;
	IDirect3DVertexBuffer9* vbPosNormals(NULL);
	IDirect3DVertexBuffer9* vbTexcoords(NULL);
	IDirect3DIndexBuffer9*	ib(NULL);
	GrMesh* result(NULL);
	D3D9MeshData* meshData(NULL);

	E_VERIFY(!_fatalError, return NULL);

	// verify input.
	uint numVerts(geometry->GetNumVerts());
	uint numTris(geometry->GetNumTris());
	E_VERIFY(numVerts != 0 && numTris != 0, return NULL);

	int channels = geometry->GetMeshChannels();
	E_VERIFY(channels & MESH_TEXCOORDS, return NULL);
	SVec2* texcoords(geometry->GetTexcoords());
	TriIdx* indices(geometry->GetIndices());
	E_VERIFY(indices != NULL, return NULL);

	// create the buffers.
	hr = gDevice->CreateVertexBuffer(4 * sizeof(SVec3) * numVerts, gManagedUsage, 0, gManagedPool, &vbPosNormals, NULL);
	E_VERIFY(SUCCEEDED(hr), goto fail);
	hr = gDevice->CreateVertexBuffer(sizeof(SVec2) * numVerts, gManagedUsage, 0, gManagedPool, &vbTexcoords, NULL);
	E_VERIFY(SUCCEEDED(hr), goto fail);
	hr = gDevice->CreateIndexBuffer(3 * sizeof(TriIdx) * numTris, gManagedUsage, D3DFMT_INDEX16, gManagedPool, &ib, NULL);
	E_VERIFY(SUCCEEDED(hr), goto fail);

	// fill the texcoord buffer.
	SVec2* vbTexcoordsPtr(NULL);
	hr = vbTexcoords->Lock(0, 0, (void **)&vbTexcoordsPtr, 0);
	E_VERIFY(SUCCEEDED(hr), return false);
	for (uint i = 0; i < numVerts; ++i)
	{
		vbTexcoordsPtr[i] = texcoords[i];
		vbTexcoordsPtr[i].Y() = 1.0f - vbTexcoordsPtr[i].Y();
	}
	vbTexcoords->Unlock();

	// fill the index buffer.
	TriIdx* ibPtr(NULL);
	hr = ib->Lock(0, 0, (void **)&ibPtr, 0);
	E_VERIFY(SUCCEEDED(hr), return false);
	for (uint i = 0; i < numTris; ++i)
	{
		*ibPtr++ = *indices++;
		*ibPtr++ = *indices++;
		*ibPtr++ = *indices++;
	}
	ib->Unlock();

	meshData = E_NEW("d3d9", D3D9MeshData);
	meshData->vbPosNormals = vbPosNormals;
	meshData->vbTexcoords = vbTexcoords;
	meshData->ib = ib;
	E_VERIFY(meshData->Update(geometry), goto fail);

	// prepare the result.
	result = E_NEW(ctx, GrMesh)(this);

	// store the skinning data.
	result->_skin = geometry;
	result->_userdata = (void*)meshData;

	// return the result.
	return result;

fail:
	E_DELETE("d3d9", meshData);
	SAFE_RELEASE(vbPosNormals);
	SAFE_RELEASE(vbTexcoords);
	SAFE_RELEASE(ib);
	return NULL;
}


//===================
// D3D9Driver::OnDestroyMesh
//===================
void		D3D9Driver::OnDestroyMesh(GrMesh& mesh)
{
	D3D9MeshData* meshData((D3D9MeshData*)mesh._userdata);
	SAFE_RELEASE(meshData->vbPosNormals);
	SAFE_RELEASE(meshData->vbTexcoords);
	SAFE_RELEASE(meshData->ib);
	E_DELETE("d3d9", meshData);
}


//===================
// D3D9Driver::CreateTexture
//===================
GrTexture*	D3D9Driver::CreateTexture(const wchar_t* ctx, const byte* bgra, uint width, uint height)
{
	E_VERIFY(!_fatalError, return NULL);
	HRESULT hr;

	// verify input.
	E_VERIFY(bgra != NULL, return NULL);
	E_VERIFY(width > 0 && height > 0, return NULL);

	// create the Direct3D9 texture.
	IDirect3DTexture9* tex(NULL);

	hr = _impl->device->CreateTexture(width, height, 1, gManagedUsage, D3DFMT_A8R8G8B8, gManagedPool, &tex, NULL);
	E_VERIFY(SUCCEEDED(hr) && "CreateTexture",
		return NULL);

	// fill the texture.
	D3DLOCKED_RECT rect;
	hr = tex->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
	E_VERIFY(SUCCEEDED(hr) && "LockRect",
	{
		tex->Release();
		return NULL;
	});

	const byte* inPixels(bgra);
	byte* outPixels((byte*)rect.pBits);
	for (uint y = 0; y < height; ++y, inPixels += 4*width, outPixels += rect.Pitch)
	{
		MemCpy(outPixels, inPixels, 4*width);
	}

	tex->UnlockRect(0);

	// prepare the result.
	GrTexture* result(E_NEW(ctx, GrTexture)(this));
	result->_userdata = (void*)tex;

	// copy the texture data.
	result->_width = width;
	result->_height = height;
	result->_pixels = BufCpy(ctx, bgra, 4*width*height);

	// return the result.
	return result;
}


//===================
// D3D9Driver::CreateTexture
//===================
GrTexture*	D3D9Driver::CreateTexture(const wchar_t* ctx, const wstring& filePath)
{
	E_VERIFY(!_fatalError, return NULL);
	HRESULT hr;

	wstring absFilePath(gFileManager->GetAbsolutePath(filePath));

	// create the texture.
	IDirect3DTexture9* tex(NULL);
	hr = D3DXCreateTextureFromFile(_impl->device, absFilePath.c_str(), &tex);
	E_VERIFY(SUCCEEDED(hr) && "CreateTexture",
		return NULL);

	// get its info.
	D3DSURFACE_DESC desc;
	hr = tex->GetLevelDesc(0, &desc);
	E_VERIFY(SUCCEEDED(hr) && "GetLevelDesc",
	{
		tex->Release();
		return NULL;
	});

	// prepare the result.
	GrTexture* result(E_NEW(ctx, GrTexture)(this));
	result->_userdata = (void*)tex;

	// copy the texture data.
	result->_width = desc.Width;
	result->_height = desc.Height;
	result->_pixels = NULL;

	// return the result.
	return result;
}


//===================
// D3D9Driver::OnDestroyTexture
//===================
void		D3D9Driver::OnDestroyTexture(GrTexture& texture)
{
	IDirect3DTexture9* d3dtex((IDirect3DTexture9*)texture._userdata);
	texture._userdata = NULL;
	if (d3dtex != NULL)
		d3dtex->Release();
}


//===================
// D3D9Driver::OnResize
//===================
void		D3D9Driver::OnResize(uint windowWidth, uint windowHeight)
{
	E_VERIFY(!_fatalError, return);

	windowWidth = E_MAX(1, windowWidth);
	windowHeight = E_MAX(1, windowHeight);

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
	pp.BackBufferCount			=  1;
	pp.MultiSampleType			= D3DMULTISAMPLE_NONE;
	pp.MultiSampleQuality		= 0;
	pp.SwapEffect				=  D3DSWAPEFFECT_DISCARD;
	pp.hDeviceWindow			= _impl->hWnd;
	pp.Flags					= 0;
	pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;
	pp.BackBufferFormat			= D3DFMT_R5G6B5; // for simplicity we'll hard-code this for now.
	pp.EnableAutoDepthStencil	= TRUE;
	pp.AutoDepthStencilFormat	= D3DFMT_D24S8;
	pp.BackBufferWidth			= windowWidth;
	pp.BackBufferHeight			= windowHeight;
	pp.Windowed					= TRUE;

	HRESULT hr = gDevice->Reset(&pp);
	if (FAILED(hr))
	{
		_fatalError = true;
		return;
	}

	const float fovy(DEGREES_TO_ANGLE(60.0f));
	const float aspect(windowWidth / (float)windowHeight);
	const float znear(1.0f);
	const float zfar(500.0f);
	D3DXMATRIX mat;
	D3DXMatrixPerspectiveFovRH(&mat, fovy, aspect, znear, zfar);
	gDevice->SetTransform(D3DTS_PROJECTION, &mat);
	gProj = mat;

	D3DXMatrixIdentity(&mat);
	gDevice->SetTransform(D3DTS_VIEW, &mat);
	gDevice->SetTransform(D3DTS_WORLD, &mat);

	gDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	gDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	gDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	gDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (uint i = 0; i < 4; ++i)
	{
		gDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		gDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
		gDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC );
	}

	SetMousePos(windowWidth/2, windowHeight/2);
	gEngine->OnMousePos(windowWidth/2, windowHeight/2);
	gEngine->OnResize(windowWidth, windowHeight);
}


//===================
// D3D9Driver::OnPaint
//===================
void		D3D9Driver::OnPaint()
{
	// render.
	_impl->device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x10101010, 1.0f, 0);
	_impl->device->BeginScene();

	gEngine->OnRender();

	_impl->device->EndScene();
	_impl->device->Present(0, 0, 0, 0);
}


//===================
// WndProc
//===================
LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM	wParam,
						 LPARAM	lParam)
{
	if (gDriver != NULL && gDriver_impl != NULL)
	{
		switch (uMsg)
		{
		case WM_ACTIVATE:
			{
				if (wParam != WA_INACTIVE)
					gDriver->SetActive(true);
				else
					gDriver->SetActive(false);
			}
			return TRUE;

		case WM_QUIT:
		case WM_CLOSE:
			gDriver_impl->done = true;
			return TRUE;

		case WM_SIZE:
			gDriver->OnResize(LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//===================
// RendererStartup
//===================
RENDER_D3D9_EXPORT void*		RendererStartup(int version,
												int windowWidth, int windowHeight,
												const wstring& windowTitle)
{
	E_VERIFY(version == GR_LIB_VERSION, return NULL);

	// return an interface to the driver.
	GrDriver* driver(E_NEW("d3d9", D3D9Driver)(windowWidth, windowHeight, windowTitle));
	return driver;
}


//===================
// RendererShutdown
//===================
RENDER_D3D9_EXPORT bool			RendererShutdown(void* driverPtr)
{
	GrDriver* driver((GrDriver*)driverPtr);
	E_DELETE("d3d9", driver);
	return true;
}

