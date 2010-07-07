//========================================================================
//	file:		import_k2_anim.cpp
//	author:		Shawn Presser 
//	date:		7/7/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "import_k2_anim.h"

// engine headers.
#include "e_file.h"

// graphics headers.
#include "gr_driver.h"
//========================================================================

//========================================================================
// ImportK2Anim_impl
//========================================================================
struct ImportK2Anim_impl
{
	int		version;
	int		numMotions;
	int		numFrames;
};

//===================
// ImportK2Anim::ImportK2Anim
//===================
ImportK2Anim::ImportK2Anim(GrDriver& driver)
: _impl(E_NEW("importk2anim", ImportK2Anim_impl))
, _driver(driver)
, _file(NULL)
{
}


//===================
// ImportK2Anim::~ImportK2Anim
//===================
ImportK2Anim::~ImportK2Anim()
{
	E_DELETE("importk2anim", _file);
	E_DELETE("importk2anim", _impl);
}


//===================
// ImportK2Anim::ParseHeader
//===================
bool			ImportK2Anim::ParseHeader(GrDriver& driver, EFile* file, uint blockSize)
{
	_impl->version = file->ReadInt();
	_impl->numMotions = file->ReadInt();
	_impl->numFrames = file->ReadInt();
	return true;
}


//===================
// ImportK2Anim::ParseBoneMotion
//===================
bool			ImportK2Anim::ParseBoneMotion(GrDriver& driver, EFile* file, uint blockSize)
{
	int boneIndex = file->ReadInt();
	int type = file->ReadInt();
	int numKeys = file->ReadInt();

	byte nameLen = file->ReadByte();
	char boneName[256];
	file->ReadArray(boneName, nameLen + 1);

	return true;
}


//===================
// ImportK2Anim::Read
//===================
bool			ImportK2Anim::Read(EFile* file)
{
	_file = file;
	E_VERIFY(file != NULL && file->IsOpen(), return false);

	uint size(file->GetFileSize());

	// check the header.
	if (size < 4)
		return false;

	{
		byte sigC(file->ReadByte());
		byte sigL(file->ReadByte());
		byte sigI(file->ReadByte());
		byte sigP(file->ReadByte());

		if ((char)sigC != 'C' || (char)sigL != 'L' || (char)sigI != 'I' || (char)sigP != 'P')
			return false;
	}

	bool checkHeader(true);

	while (file->GetPos() + 4 + sizeof(int) < file->GetFileSize())
	{
		char blockName[5];
		for (int i = 0; i < 4; ++i)
			blockName[i] = (char)file->ReadByte();
		blockName[4] = 0;

		int blockSize = file->ReadInt();

		uint filePos(file->GetPos());

		if (checkHeader)
		{
			checkHeader = false;
			E_VERIFY(strncmp(blockName, "head", 4) == 0, return false);
			ParseHeader(_driver, _file, (uint)blockSize);
		}
		else if (strncmp(blockName, "bmtn", 4) == 0)
			ParseBoneMotion(_driver, _file, (uint)blockSize);

		file->Seek(filePos + blockSize);
	}
	file->Close();

	return true;
}


