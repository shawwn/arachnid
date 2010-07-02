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
//========================================================================


//===================
// main
//===================
#if E_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetMessage // goddamn windows.h macros
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	// initialize the engine
	try
	{
		// startup system
		new ESystem;
		//gSystem->DisplayMessage(_T("Startup"), _T("Congrats!  You have started the Arachnid engine."));

		// startup subsystems
		new EFileManager;

		// startup the engine
		wstring renderer(E_PLATFORM_RENDERER);
		renderer = _T("gl2");
		new EEngine(renderer);

		// main engine loop
		while (gEngine->PerFrame())
		{
			// relinquish some CPU time.
			gSystem->Sleep(10);
		}

		// shutdown the engine
		delete gEngine;

		// shutdown subsystems
		delete gFileManager;

		// shutdown system
		//gSystem->DisplayMessage(_T("Fission Mailed"), _T("Congrats!  You ran the Arachnid engine."));
		//gSystem->DisplayMessage(_T("WTF"), _T("Unfortunately, the engine doesn't even do anything yet."));
		//gSystem->DisplayMessage(_T("Q_Q"), _T("Owned."));
		delete gSystem;
	}
	catch (const EException& ex)
	{
		gSystem->DisplayMessage(_T("Initialization Failed"), _TS("Failed to initialize the engine.  Error:\n\n") + ex.GetMessage());
	}

	return 0;
}
