//========================================================================
//	file:		r_d3d9_driver.cpp
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "r_d3d9_common.h"
#include "r_d3d9_driver.h"

// graphics headers.
#include "../engine/gr_driver.h"
#include "../engine/gr_scene.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_texture.h"
#include "../engine/gr_material.h"

// d3d9 headers.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
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
D3D9Driver*			gDriver;
D3D9Driver_impl*	gDriver_impl;
IDirect3DTexture9*	gNullTex;
//========================================================================

//========================================================================
// D3D9Vertex
//========================================================================
struct D3D9Vertex
{
	SVec3			pos;
	SVec2			uv;
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
};
//========================================================================


//===================
// D3D9Driver::RenderModelNode
//===================
void		D3D9Driver::RenderModelNode(const GrModelNode& node)
{
	const MMat44& transform(node.GetTransform());

	GrMesh* mesh(node.GetMesh());
	if (mesh != NULL)
	{
		_impl->device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

		const SVec3* positions(mesh->GetPositions());
		const SVec2* texcoords(mesh->GetTexcoords());
		const TriIdx* indices(mesh->GetTriIndices());

		for (uint rangeIdx = 0; rangeIdx < node.NumMeshRanges(); ++rangeIdx)
		{
			GrModelNode::SMeshRange* range(node.GetMeshRange(rangeIdx));
			E_VERIFY(range != NULL, continue);

			// set texture.
			GrTexture* diffuse(range->material->GetTexture(MT_DIFFUSE));
			if (diffuse != NULL)
				_impl->device->SetTexture(0, (IDirect3DBaseTexture9*)diffuse->_userdata);
			else
				_impl->device->SetTexture(0, gNullTex);

			for (uint tri = range->triStart; tri < range->triStart + range->triCount; ++tri)
			{
				D3D9Vertex vert[3];
				for (uint corner = 0; corner < 3; ++corner)
				{
					uint idx(indices[3*tri + corner]);
					vert[corner].pos = positions[idx];
					vert[corner].uv = texcoords[idx];
					vert[corner].pos.RotateTranslate(transform);
					vert[corner].uv.Y() = 1.0f - vert[corner].uv.Y();
				}
				HRESULT hr = _impl->device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, (const void*)vert, sizeof(D3D9Vertex));
				if (FAILED(hr))
				{
					_fatalError = true;
					return;
				}
			}
		}
	}

	// render each child.
	for (uint i = 0; i < node.NumChildModelNodes(); ++i)
	{
		const GrModelNode& child(*node.GetChildModelNode(i));
		RenderModelNode(child);
	}
}


//===================
// D3D9Driver::RenderModel
//===================
void		D3D9Driver::RenderModel(const GrModel& model)
{
	// render the nodes.
	RenderModelNode(model.GetRoot());

	// render each child.
	for (uint i = 0; i < model.NumChildModels(); ++i)
	{
		const GrModel& child(*model.GetChildModel(i));
		RenderModel(child);
	}
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
	_impl->d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
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
	pp.EnableAutoDepthStencil	= FALSE;
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

	ShowWindow(_impl->hWnd, SW_SHOW);
}


//===================
// D3D9Driver::~D3D9Driver
//===================
D3D9Driver::~D3D9Driver()
{
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
// D3D9Driver::CreateMesh
//===================
GrMesh*		D3D9Driver::CreateMesh(const wchar_t* ctx,
							   const SVec3* positions, const SVec2* texcoords, uint numVerts,
							   const TriIdx* triangles, uint numTris)
{
	E_VERIFY(!_fatalError, return NULL);

	// verify input.
	E_VERIFY(numVerts != 0 && numTris != 0, return NULL);
	E_VERIFY(positions != NULL && triangles != NULL, return NULL);

	// prepare the result.
	GrMesh* result(E_NEW(ctx, GrMesh)(this));

	// store the vertex data.
	result->_positions = ArrayCpy(ctx, positions, numVerts);
	result->_texcoords = ArrayCpy(ctx, texcoords, numVerts);
	result->_numVertices = numVerts;

	// store the index data.
	result->_triIndices = ArrayCpy(ctx, triangles, 3*numTris);
	result->_numTriangles = numTris;

	// return the result.
	return result;
}


//===================
// D3D9Driver::OnDestroyMesh
//===================
void		D3D9Driver::OnDestroyMesh(GrMesh& mesh)
{
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

	hr = _impl->device->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL);
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
	result->_pixels = ArrayCpy(ctx, bgra, 4*width*height);

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
	pp.EnableAutoDepthStencil	= FALSE;
	pp.BackBufferWidth			= windowWidth;
	pp.BackBufferHeight			= windowHeight;
	pp.Windowed					= TRUE;

	HRESULT hr = _impl->device->Reset(&pp);
	if (FAILED(hr))
	{
		_fatalError = true;
		return;
	}

	const float fovy(DEG2RAD(60.0f));
	const float aspect(windowWidth / (float)windowHeight);
	const float znear(1.0f);
	const float zfar(100.0f);
	D3DXMATRIX mat;
	D3DXMatrixPerspectiveFovRH(&mat, fovy, aspect, znear, zfar);
	_impl->device->SetTransform(D3DTS_PROJECTION, &mat);

	D3DXMatrixIdentity(&mat);
	_impl->device->SetTransform(D3DTS_VIEW, &mat);
	_impl->device->SetTransform(D3DTS_WORLD, &mat);

	_impl->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	_impl->device->SetRenderState(D3DRS_LIGHTING, FALSE);
	_impl->device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	_impl->device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	_impl->device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
}


//===================
// D3D9Driver::OnPaint
//===================
void		D3D9Driver::OnPaint()
{
	// render.
	_impl->device->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	_impl->device->BeginScene();

	// render the scene.
	RenderModel(GetScene().GetModel());

	_impl->device->EndScene();
	_impl->device->Present(0, 0, 0, 0);

	// swap front/back buffer.
	if (_impl->hDC != NULL)
		SwapBuffers(_impl->hDC);
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
		case WM_QUIT:
		case WM_CLOSE:
			gDriver_impl->done = true;
			break;

		case WM_PAINT:
			gDriver->OnPaint();
			break;

		case WM_SIZE:
			gDriver->OnResize(LOWORD(lParam), HIWORD(lParam));
			break;
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

