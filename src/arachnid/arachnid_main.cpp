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

// math headers.
#include "../engine/m_vec3.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define ARACHNID_RENDERER			_T("gl2")
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

// textures.
GrTexture*	gCheckerTex;
GrMaterial*	gChecker;
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
		SVec3 positions[] = {
			SVec3( 0.0f, 1.0f, 0.0f),
			SVec3( 1.0f,-1.0f, 0.0f),
			SVec3(-1.0f,-1.0f, 0.0f)
		};
		SVec2 texcoords[] = {
			SVec2( 0.5f, 1.0f),
			SVec2( 1.0f, 0.0f),
			SVec2( 0.0f, 0.0f)
		};
		TriIdx triangles[] = { 0, 1, 2 };
		uint numVerts(sizeof(positions) / sizeof(SVec3));
		uint numTris((sizeof(triangles)/sizeof(TriIdx)) / 3);
		gMeshTri = renderer.CreateMesh(_T("main"),
			positions, texcoords, numVerts,
			triangles, numTris);

		if (gMeshTri != NULL)
		{
			gTri = GrModel::Create(_T("main"), _T("triangle"));
			if (gTri != NULL)
			{
				GrModelNode& root(gTri->GetRoot());
				root.SetTransform(MMat44::Translation(MVec3(-1.5f, 0.0f, -6.0f)));
				root.SetMesh(gMeshTri);
				root.AddMeshRange(GrModelNode::SMeshRange(0, numTris, gChecker));
				sceneModel.AddChildModel(gTri);
			}
		}
	}

	// create square mesh.
	{
		SVec3 positions[] = {
			SVec3(-1.0f, 1.0f, 0.0f),
			SVec3( 1.0f, 1.0f, 0.0f),
			SVec3( 1.0f,-1.0f, 0.0f),
			SVec3(-1.0f,-1.0f, 0.0f)
		};
		SVec2 texcoords[] = {
			SVec2( 0.0f, 1.0f),
			SVec2( 1.0f, 1.0f),
			SVec2( 1.0f, 0.0f),
			SVec2( 0.0f, 0.0f)
		};
		TriIdx triangles[] = {
			0, 1, 2,
			0, 2, 3
		};
		uint numVerts(sizeof(positions) / sizeof(SVec3));
		uint numTris((sizeof(triangles)/sizeof(TriIdx)) / 3);
		gMeshSquare = renderer.CreateMesh(_T("main"),
			positions, texcoords, numVerts,
			triangles, numTris);

		if (gMeshSquare != NULL)
		{
			gSquare = GrModel::Create(_T("main"), _T("square"));
			if (gSquare != NULL)
			{
				GrModelNode& root(gSquare->GetRoot());
				root.SetTransform(MMat44::Translation(MVec3(1.5f, 0.0f, -6.0f)));
				root.SetMesh(gMeshSquare);
				root.AddMeshRange(GrModelNode::SMeshRange(0, numTris, gChecker));
				sceneModel.AddChildModel(gSquare);
			}
		}
	}

	return true;
}


//===================
// ShutdownRenderer
//===================
void			ShutdownRenderer()
{
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
	//gSystem->DisplayMessage(_T("Startup"), _T("Congrats!  You have started the Arachnid engine."));

	// startup subsystems
	E_NEW("main", EFileManager);

	// startup the engine
	wstring renderer(E_PLATFORM_RENDERER);
	renderer = ARACHNID_RENDERER;
	EEngine::Create(_T("main"), renderer);

	// startup the renderer.
	if (StartupRenderer())
	{
		// main engine loop
		while (gEngine->PerFrame())
		{
			// relinquish some CPU time.
			gSystem->Sleep(10);
		}
	}

	// shutdown the renderer.
	ShutdownRenderer();

	// shutdown the engine
	E_DELETE("main", gEngine);

	// shutdown subsystems
	E_DELETE("main", gFileManager);

	// shutdown system
	//gSystem->DisplayMessage(_T("Fission Mailed"), _T("Congrats!  You ran the Arachnid engine."));
	//gSystem->DisplayMessage(_T("WTF"), _T("Unfortunately, the engine doesn't even do anything yet."));
	//gSystem->DisplayMessage(_T("Q_Q"), _T("Owned."));
	E_DELETE("main", gSystem);

	//gSystem->DisplayMessage(_T("Initialization Failed"), _TS("Failed to initialize the engine.  Error:\n\n") + ex.GetMessage());

	return 0;
}
