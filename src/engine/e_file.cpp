//========================================================================
//	file:		e_file.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_file.h"
//========================================================================

//===================
// EFileDisk::IsModeValid
//===================
bool		EFile::IsModeValid(uint mode) const
{
	bool read = (mode & FILE_READ) != 0;
	bool write = (mode & FILE_WRITE) != 0;
	bool binary = (mode & FILE_BINARY) != 0;
	bool text = (mode & FILE_TEXT) != 0;

	if (read && write)
		return false;

	if (binary && text)
		return false;

	return true;
}


//===================
// EFile::EFile
//===================
EFile::EFile()
: _mode(0)
, _textEncoding(TE_UTF8)
{
}

