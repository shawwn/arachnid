//========================================================================
//	file:		e_filedisk.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_filedisk.h"
//========================================================================


//===================
// EFileDisk::EFileDisk
//===================
EFileDisk::EFileDisk()
: _fp(NULL)
, _fileSize(0)
{
}


//===================
// EFileDisk::~EFileDisk
//===================
EFileDisk::~EFileDisk()
{
	Close();
}


//===================
// EFileDisk::Open
//===================
bool		EFileDisk::Open(const wstring& path, uint mode)
{
	if (!EFile::Open(path, mode))
		return false;

	// open the file.
	if (HasMode(FILE_READ))
		_fp = _wfopen(path.c_str(), _T("rb"));
	else if (HasMode(FILE_WRITE))
		if (HasMode(FILE_APPEND))
			_fp = _wfopen(path.c_str(), _T("wb+"));
		else
			_fp = _wfopen(path.c_str(), _T("wb"));

	E_VERIFY(_fp != NULL, return false);

	// determine the file size.
	fseek(_fp, 0, SEEK_END);
	_fileSize = (uint)ftell(_fp);
	fseek(_fp, 0, SEEK_SET);

	// determine text encoding.
	if (!OnFileOpened())
	{
		Close();
		return false;
	}

	return true;
}


//===================
// EFileDisk::Close
//===================
void		EFileDisk::Close()
{
	if (_fp != NULL)
	{
		fclose(_fp);
		_fp = NULL;
	}

	_fileSize = 0;
	_mode = 0;
}


//===================
// EFileDisk::IsOpen
//===================
bool		EFileDisk::IsOpen() const
{
	if (_fp == NULL)
		return false;
	return true;
}


//===================
// EFileDisk::IsEOF
//===================
bool		EFileDisk::IsEOF() const
{
	if (HasMode(FILE_READ))
	{
		if (_fp == NULL)
			return true;

		if (GetPos() >= _fileSize)
			return true;

		return false;
	}

	E_VERIFY(HasMode(FILE_WRITE), return true);

	return false;
}


//===================
// EFileDisk::GetPos
//===================
uint		EFileDisk::GetPos() const
{
	E_VERIFY(_fp != NULL, return 0);

	uint pos((uint)ftell(_fp));
	return pos;
}


//===================
// EFileDisk::Seek
//===================
bool		EFileDisk::Seek(uint pos)
{
	E_VERIFY(_fp != NULL, return 0);

	if (fseek(_fp, pos, SEEK_SET) != 0)
		return false;

	return true;
}


//===================
// EFileDisk::Read
//===================
uint		EFileDisk::Read(byte* outBuf, uint count)
{
	E_VERIFY(_fp != NULL, return 0);

	E_VERIFY(HasMode(FILE_READ), return 0);

	int bytesRead(fread(outBuf, 1, count, _fp));
	if (bytesRead < 0)
		return 0;

	return bytesRead;
}


//===================
// EFileDisk::Write
//===================
uint		EFileDisk::Write(const byte* buf, uint bufSize)
{
	E_VERIFY(_fp != NULL, return 0);

	E_VERIFY(HasMode(FILE_WRITE), return 0);

	uint numWritten = (uint)fwrite(buf, 1, bufSize, _fp);
	return numWritten;
}


//===================
// EFileDisk::GetFileSize
//===================
uint		EFileDisk::GetFileSize() const
{
	return _fileSize;
}
