//========================================================================
//	file:		e_filemem.h
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
// EFileMem
//========================================================================
class EFileMem : public EFile
{
private:
	byte*			_fileBuf;
	uint			_fileBufReserved;
	uint			_fileSize;
	uint			_filePos;
	uint			_growSize; // the number of bytes to grow by when writing.
	bool			_ownMem; // determines whether to delete the filebuf.

	uint				Grow(uint totalSize);
public:
	// ctor & dtor.
	EFileMem();
	virtual ~EFileMem();

	// per-file methods.
	virtual bool		Open(const wstring& path, uint mode = FILE_READ);
	virtual bool		OpenMemForReading(const byte* buf, uint bufSize, uint mode = 0);
	virtual bool		OpenFixedMemForWriting(byte* buf = NULL, uint bufSize = 0, uint mode = 0);
	virtual bool		OpenMemForWriting(uint mode = 0, uint reserveSize = 0);
	virtual void		Close();
	virtual bool		IsOpen() const;
	virtual bool		IsEOF() const;

	virtual uint		GetPos() const;
	virtual bool		Seek(uint pos);

	virtual uint		Read(byte* outBuf, uint count);

	virtual uint		Write(const byte* buf, uint bufSize);

	virtual uint		GetFileSize() const;
	virtual const byte*	GetFileBuf() const;
};
//========================================================================


