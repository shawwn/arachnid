//========================================================================
//	file:		arachnid_main.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "../engine/e_common.h"

// engine headers.
#include "../engine/e_system.h"
#include "../engine/e_filemanager.h"
#include "../engine/e_engine.h"

// graphics headers.
#include "../engine/gr_scene.h"
#include "../engine/gr_model.h"
#include "../engine/gr_model_node.h"
#include "../engine/gr_mesh.h"
#include "../engine/gr_material.h"
#include "../engine/gr_texture.h"
#include "../engine/gr_camera.h"
#include "../engine/gr_anim_mixer.h"

// math headers.
#include "../engine/m_vec3.h"

// import headers.
#include "../engine/import_k2_model.h"
#include "../engine/import_k2_anim.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define ARACHNID_RENDERER			_T("gl2")

#define WORLD_SIDE					MVec3(1.0f, 0.0f, 0.0f)
#define WORLD_UP					MVec3(0.0f, 0.0f, 1.0f)
#define WORLD_FORWARD				MVec3(0.0f, 1.0f, 0.0f)

#define CAM_POS						MVec3(0.0f, -150.0f, 0.0f)

#define MODEL_BASE_PATH				_TS("game/models/witch_slayer/")
#define MODEL_TEXTURE				_TS("color.tga")
#define MODEL_ANIM					_TS("clips/walk_1.clip")
#define MODEL_MDL					_TS("high.model")
//========================================================================

//========================================================================
// Globals
//========================================================================

// square model.
GrModel*	gSquare;
GrMesh*		gMeshSquare;

// triangle model.
GrModel*	gTri;
GrMesh*		gMeshTri;

// checker material.
GrTexture*	gCheckerTex;
GrMaterial*	gChecker;

// ws material.
GrModel*	gWsMdl;
GrTexture*	gWsTex;
GrMaterial*	gWsMat;
//========================================================================

//===================
// StartupRenderer
//===================
bool			StartupRenderer()
{
	E_VERIFY(gEngine != NULL, return false);
	GrDriver& renderer(gEngine->GetRenderer());
	GrModel& sceneModel(gEngine->GetScene().GetModel());

	// create checkerboard texture.
	{
		uint checker[] = {
			0xFFFF0000,		0xFF00FF00,
			0xFF0000FF,		0xFFFFFFF
		};

		gCheckerTex = renderer.CreateTexture(_T("main"), (const byte*)checker, 2, 2);
		E_VERIFY(gCheckerTex != NULL, return false);

		gChecker = E_NEW("main", GrMaterial);
		gChecker->SetTexture(MT_DIFFUSE, gCheckerTex);
	}

	// create tri mesh.
	{
		GrSkin* skin(E_NEW("main", GrSkin));
		skin->StartVert(SVec3( 0.0f, 1.0f, 0.0f), SVec2( 0.5f, 1.0f));
		skin->StartVert(SVec3( 1.0f,-1.0f, 0.0f), SVec2( 1.0f, 0.0f));
		skin->StartVert(SVec3(-1.0f,-1.0f, 0.0f), SVec2( 0.0f, 0.0f));
		skin->AddTriangle(0, 1, 2);
		gMeshTri = renderer.CreateMesh(_T("main"), skin);

		if (gMeshTri != NULL)
		{
			gTri = GrModel::Create(_T("main"), _T("triangle"));
			if (gTri != NULL)
			{
				GrModelNode& root(gTri->GetRoot());
				root.SetLocal(MTransform(MVec3(-1.5f, 0.0f, -6.0f)));
				root.SetMesh(gMeshTri);
				root.AddMeshRange(GrModelNode::SMeshRange(0, skin->GetNumTris(), gChecker));
				sceneModel.AddChildModel(gTri);
			}
		}
	}

	// create square mesh.
	{
		GrSkin* skin(E_NEW("main", GrSkin));
		skin->StartVert(SVec3(-1.0f, 1.0f, 0.0f), SVec2( 0.0f, 1.0f));
		skin->StartVert(SVec3( 1.0f, 1.0f, 0.0f), SVec2( 1.0f, 1.0f));
		skin->StartVert(SVec3( 1.0f,-1.0f, 0.0f), SVec2( 1.0f, 0.0f));
		skin->StartVert(SVec3(-1.0f,-1.0f, 0.0f), SVec2( 0.0f, 0.0f));
		skin->AddTriangle(0, 1, 2);
		skin->AddTriangle(0, 2, 3);
		gMeshSquare = renderer.CreateMesh(_T("main"), skin);

		if (gMeshSquare != NULL)
		{
			gSquare = GrModel::Create(_T("main"), _T("square"));
			if (gSquare != NULL)
			{
				GrModelNode& root(gSquare->GetRoot());
				root.SetLocal(MTransform(MVec3(1.5f, 0.0f, -6.0f)));
				root.SetMesh(gMeshSquare);
				root.AddMeshRange(GrModelNode::SMeshRange(0, skin->GetNumTris(), gChecker));
				sceneModel.AddChildModel(gSquare);
			}
		}
	}

	// create material.
	{
		gWsMat = E_NEW(_T("main"), GrMaterial);
		gWsTex = renderer.CreateTexture(_T("main"), _TS(FILE_PATH_ROOT_DIR) + MODEL_BASE_PATH + MODEL_TEXTURE);
		if (gWsTex != NULL)
			gWsMat->SetTexture(MT_DIFFUSE, gWsTex);
		else
			gWsMat->SetTexture(MT_DIFFUSE, gCheckerTex);
	}

	// import model.
	GrModel* model(NULL);
	{
		EFile* file = gFileManager->GetFile(_TS(FILE_PATH_ROOT_DIR) + MODEL_BASE_PATH + MODEL_MDL, FILE_READ | FILE_MEMORY);

		ImportK2Model import(renderer, gWsMat);
		if (import.Read(file))
		{
			GrModel& sceneModel(renderer.GetScene().GetModel());
			model = import.GetModel();
			sceneModel.AddChildModel(model);
			gWsMdl = model;
		}
	}

	// import anim.
	if (model != NULL)
	{
		EFile* file = gFileManager->GetFile(_TS(FILE_PATH_ROOT_DIR) + MODEL_BASE_PATH + MODEL_ANIM, FILE_READ | FILE_MEMORY);

		ImportK2Anim import(renderer);
		if (GrAnim* anim = import.Read(file))
		{
			gWsMdl->Animations()->PlayAnim(anim);
		}
	}
	gEngine->GetCamera().SetPos(CAM_POS);

	return true;
}


//===================
// ShutdownRenderer
//===================
void			ShutdownRenderer()
{
	E_DELETE("main", gWsMat);
	E_DELETE("main", gWsTex);

	E_DELETE("main", gChecker);
	E_DELETE("main", gCheckerTex);

	E_DELETE("main", gMeshTri);
	E_DELETE("main", gMeshSquare);
}


//===================
// main
//===================
#if E_WINDOWS
#include <windows.h>
#undef GetMessage // goddamn windows.h macros
int WINAPI		WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int				main()
#endif
{
	// startup system
	E_NEW("main", ESystem);

	// startup subsystems
	E_NEW("main", EFileManager);

	// startup the engine
	wstring renderer(E_PLATFORM_RENDERER);
	renderer = ARACHNID_RENDERER;

	SEngineConfig config;
	config.side = WORLD_SIDE;
	config.up = WORLD_UP;
	config.forward = WORLD_FORWARD;
	config.renderer = renderer;

	EEngine::Create(_T("main"), config);

	// startup the renderer.
	if (StartupRenderer())
	{
		gSystem->TimeStart();

		uint timeLast = gSystem->TimeElapsed();
		uint timeNow = timeLast;
		uint dt = 0;

		// main engine loop
		while (gEngine->PerFrame(dt))
		{
			// relinquish some CPU time.
			gSystem->Sleep(10);

			timeLast = timeNow;
			timeNow = gSystem->TimeElapsed();
			dt = timeNow - timeLast;
		}
	}

	// shutdown the renderer.
	ShutdownRenderer();

	// shutdown the engine
	E_DELETE("main", gEngine);

	// shutdown subsystems
	E_DELETE("main", gFileManager);

	// shutdown system
	E_DELETE("main", gSystem);

	return 0;
}
