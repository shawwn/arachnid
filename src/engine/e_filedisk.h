//========================================================================
//	file:		e_filedisk.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Headers
//========================================================================
#include "e_file.h"
//========================================================================

//========================================================================
// EFileDisk
//========================================================================
class EFileDisk : public EFile
{
private:
	FILE*		_fp;
	uint		_fileSize;
public:
	// ctor & dtor.
	EFileDisk();
	virtual ~EFileDisk();

	// per-file methods.
	virtual bool		Open(const wstring& path, uint mode = FILE_READ);
	virtual void		Close();
	virtual bool		IsOpen() const;
	virtual bool		IsEOF() const;

	virtual uint		GetPos() const;
	virtual void		Seek(uint pos);

	virtual uint		Read(byte* outBuf, uint count);
	virtual wstring		ReadLine();

	virtual uint		Write(const byte* buf, uint bufSize);
	virtual uint		WriteLine(const wstring& str);
	virtual uint		WriteLine(const string& str);

	virtual uint		GetFileSize() const;
	virtual const byte*	GetFileBuffer() const;
};
//========================================================================


