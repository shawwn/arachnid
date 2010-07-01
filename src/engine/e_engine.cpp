//========================================================================
//	file:		e_engine.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_engine.h"
#include "e_system.h"
#include "e_filemanager.h"
//========================================================================

//========================================================================
// Definitions
//========================================================================
EEngine*	gEngine;
//========================================================================

//===================
// EEngine::EEngine
//===================
EEngine::EEngine()
{
	new ESystem;
	new EFileManager;

	EFile* file(gFileManager->GetFile(_T(":test.txt"), FILE_READ | FILE_TEXT));
	if (file != NULL)
	{
		wstrvec lines;
		while (!file->IsEOF())
			lines.push_back(file->ReadLine());
		file->Close();
		delete file;
	}

	gEngine = this;
}


//===================
// EEngine::~EEngine
//===================
EEngine::~EEngine()
{
	delete gFileManager;
	delete gSystem;

	gEngine = NULL;
}

