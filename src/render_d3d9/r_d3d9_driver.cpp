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
//========================================================================

//========================================================================
// D3D9Vertex
//========================================================================
struct D3D9Vertex
{
	SVec3			pos;

	union
	{
		byte			bgra[4];
		uint			color;
	};

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
// D3D9Driver::RenderMeshNormals
//===================
void		D3D9Driver::RenderMeshNormals(GrMesh* mesh)
{
	if (mesh == NULL)
		return;

	GrSkin* skin(mesh->GetSkin());
	const byte* boneIndices(NULL);
	const float* boneWeights(NULL);
	const MTransform** skeleton = NULL;
	const SVec3* positions(skin->GetPositions());
	const SVec3* normals(skin->GetNormals());
	const SVec3* binormals(skin->GetBinormals());
	const SVec3* tangents(skin->GetTangents());

	if (normals == NULL && binormals == NULL && tangents == NULL)
		return;

	_impl->device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	_impl->device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	_impl->device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	uint numVerts(skin->GetNumVerts());
	for (uint vertIdx = 0; vertIdx < numVerts; ++vertIdx)
	{
		const SVec3& pos(positions[vertIdx]);

		int numVecs = 0;
		SVec3 vecs[3];
		uint colors[3];

		if (normals != NULL)
		{
			uint i = numVecs++;
			vecs[i] = normals[vertIdx];
			colors[i] = 0xFF00FF00;
		}

		if (binormals != NULL)
		{
			uint i = numVecs++;
			vecs[i] = binormals[vertIdx];
			colors[i] = 0xFF0000FF;
		}

		if (tangents != NULL)
		{
			uint i = numVecs++;
			vecs[i] = tangents[vertIdx];
			colors[i] = 0xFFFF0000;
		}

		E_ASSERT(numVecs > 0);
		for (int i = 0; i < numVecs; ++i)
		{
			D3D9UntexturedVertex verts[2];

			verts[0].color = colors[i];
			verts[1].color = colors[i];

			verts[0].pos = pos;
			verts[1].pos = pos + 3.0f*vecs[i];

			HRESULT hr = _impl->device->DrawPrimitiveUP(D3DPT_LINELIST, 1, (const void*)verts, sizeof(D3D9UntexturedVertex));
			if (FAILED(hr))
			{
				_fatalError = true;
				return;
			}
		}
	}
}


//===================
// D3D9Driver::RenderModelNode
//===================
void		D3D9Driver::RenderModelNode(GrModel& parent, GrModelNode& node)
{
	GrMesh* mesh(node.GetMesh());
	if (mesh != NULL)
	{
		GrSkin* skin(mesh->GetSkin());
		const byte* boneIndices(NULL);
		const float* boneWeights(NULL);
		const MTransform** skeleton = NULL;
		const SVec3* positions(skin->GetPositions());
		if (skin->GetMeshChannels() & MESH_BONE_INFO)
		{
			skeleton = node.GetSkeleton();
			positions = skin->DeformVerts(skeleton);
		}

		_impl->device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		_impl->device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		_impl->device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		_impl->device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);

		const SVec2* texcoords(skin->GetTexcoords());
		const TriIdx* indices(skin->GetIndices());

		E_ASSERT(node.NumMeshRanges() > 0);
		for (uint rangeIdx = 0; rangeIdx < node.NumMeshRanges(); ++rangeIdx)
		{
			GrModelNode::SMeshRange* range(node.GetMeshRange(rangeIdx));
			E_VERIFY(range != NULL, continue);

			// set texture.
			GrTexture* diffuse(range->material->GetTexture(MT_DIFFUSE));
			if (diffuse != NULL)
			{
				_impl->device->SetTexture(0, (IDirect3DBaseTexture9*)diffuse->_userdata);
				gShaderConstants->GetPixelConstant(PC_DIFFUSE).SetTexture((IDirect3DBaseTexture9*)diffuse->_userdata);
			}
			else
			{
				_impl->device->SetTexture(0, gNullTex);
				gShaderConstants->GetPixelConstant(PC_DIFFUSE).SetTexture(gNullTex);
			}

			// set shader.
			gShaderSimple->Apply();

			for (uint tri = range->triStart; tri < range->triStart + range->triCount; ++tri)
			{
				D3D9Vertex vert[3];

				const MTransform& xform(node.GetWorld());

				for (uint corner = 0; corner < 3; ++corner)
				{
					uint idx(indices[3*tri + corner]);
					vert[corner].pos = positions[idx];
					vert[corner].uv = texcoords[idx];
					vert[corner].pos.RotateTranslate(xform);
					vert[corner].uv.Y() = 1.0f - vert[corner].uv.Y();
					vert[corner].color = 0xFFFFFFFF;
				}

				HRESULT hr = _impl->device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, (const void*)vert, sizeof(D3D9Vertex));
				if (FAILED(hr))
				{
					_fatalError = true;
					return;
				}
			}

			gDevice->SetVertexShader(NULL);
			gDevice->SetPixelShader(NULL);
		}

#if RENDER_NORMALS
		RenderMeshNormals(mesh);
#endif
	}

	// render each child.
	for (uint i = 0; i < node.NumChildModelNodes(); ++i)
	{
		GrModelNode& child(*node.GetChildModelNode(i));
		RenderModelNode(parent, child);
	}
}


//===================
// D3D9Driver::RenderModel
//===================
void		D3D9Driver::RenderModel(GrModel& model)
{
	// render the nodes.
	RenderModelNode(model, model.GetRoot());

	// render each child.
	for (uint i = 0; i < model.NumChildModels(); ++i)
	{
		GrModel& child(*model.GetChildModel(i));
		RenderModel(child);
	}

#if E_PRINT_DEBUG_INFO
	model.PrintDebug();
#endif

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

	// create a shader.
	wstring shaderErrors;
	gShaderSimple = RD3D9Shader::Create(_T("simple"), shaderErrors);
}


//===================
// D3D9Driver::~D3D9Driver
//===================
D3D9Driver::~D3D9Driver()
{
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
	E_VERIFY(!_fatalError, return NULL);

	// verify input.
	E_VERIFY(geometry->GetNumVerts() != 0 && geometry->GetNumTris() != 0, return NULL);

	// prepare the result.
	GrMesh* result(E_NEW(ctx, GrMesh)(this));

	// store the skinning data.
	result->_skin = geometry;

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
	const float zfar(10000.0f);
	D3DXMATRIX mat;
	D3DXMatrixPerspectiveFovRH(&mat, fovy, aspect, znear, zfar);
	gDevice->SetTransform(D3DTS_PROJECTION, &mat);
	gProj = mat;

	D3DXMatrixIdentity(&mat);
	gDevice->SetTransform(D3DTS_VIEW, &mat);
	gDevice->SetTransform(D3DTS_WORLD, &mat);

	gDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	gDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	gDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	gDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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

	// apply the camera.
	GrCamera* cam(GetCamera());
	if (cam != NULL)
	{
		ApplyCamera(*cam);

		// set shader constants.
		gShaderConstants->GetVertexConstant(VC_WORLD_VIEW_PROJ).SetMatrix(gView * gProj);
		gShaderConstants->GetPixelConstant(PC_DIFFUSE_COLOR).SetVector(D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		// render the scene.
		RenderModel(GetScene().GetModel());
	}

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

