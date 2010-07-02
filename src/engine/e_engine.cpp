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
#include "m_vec3.h"
#include "m_mat33.h"
#include "m_mat44.h"
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

	// test filesystem.
	EFile* file(gFileManager->GetFile(_T(":test.txt"), FILE_READ | FILE_TEXT));
	if (file != NULL)
	{
		wstrvec lines;
		while (!file->IsEOF())
			lines.push_back(file->ReadLine());
		file->Close();
		delete file;
	}

	// test matrices.
	MVec3	pt(1.0f, 0.0f, 0.0f);
	MMat33	rotX(MMat33::XRot(M_EIGTH_TURN));
	MMat33	rotY(MMat33::YRot(M_EIGTH_TURN));
	MMat33	rotZ(MMat33::ZRot(M_EIGTH_TURN));
	MMat33	rotZY(rotY * rotZ);

	rotZ.Rotate(pt);
	rotY.Rotate(pt);

	pt.Set(1.0f, 0.0f, 0.0f);
	rotZY.Rotate(pt);

	pt.Set(1.0f, 0.0f, 0.0f);
	MMat44  trans;
	trans.SetRot(rotZY);
	trans.RotateTranslate(pt);

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

