//========================================================================
//	file:		e_filemem.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_filemem.h"
//========================================================================

//========================================================================
// Constants
//========================================================================
#define FILE_MEM_GROW_SIZE		1024
#define FILE_MEM_GROW_MASK		~0x3FF
//========================================================================


//===================
// EFileMem::Grow
//===================
uint		EFileMem::Grow(uint totalSize)
{
	E_VERIFY(HasMode(FILE_WRITE), return false);

	if (_ownMem)
	{
		if (totalSize > _fileBufReserved)
		{
			uint nextSize(totalSize);
			if ((nextSize & FILE_MEM_GROW_MASK) != totalSize)
				nextSize = (totalSize + FILE_MEM_GROW_SIZE) & FILE_MEM_GROW_MASK;

			BufResize(_T("filemem"), _fileBuf, _fileBufReserved, nextSize);
			_fileBufReserved = nextSize;
		}
	}

	// if the grow failed, abort.
	if (totalSize > _fileBufReserved)
		return 0;

	// return the number of bytes remaining.
	return _fileBufReserved - GetPos();
}


//===================
// EFileMem::EFileMem
//===================
EFileMem::EFileMem()
: _fileBuf(NULL)
, _fileBufReserved(0)
, _fileSize(0)
, _filePos(0)
, _growSize(FILE_MEM_GROW_SIZE)
, _ownMem(true)
{
}


//===================
// EFileMem::~EFileMem
//===================
EFileMem::~EFileMem()
{
	Close();
}


//===================
// EFileMem::Open
//===================
bool		EFileMem::Open(const wstring& path, uint mode)
{
	// sanity check.
	E_VERIFY(_fileBuf == NULL, E_DELETE_ARRAY("filemem", _fileBuf));

	if (!EFile::Open(path, mode))
		return false;

	_fileSize = 0;
	_fileBufReserved = 0;
	_growSize = FILE_MEM_GROW_SIZE;

	if (HasMode(FILE_READ) || HasMode(FILE_APPEND))
	{
		// open the file.
		FILE* fp = _wfopen(path.c_str(), _T("rb"));
		E_VERIFY(fp != NULL, return false);

		// determine the file size.
		fseek(fp, 0, SEEK_END);
		_fileSize = (uint)ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// read the file into memory.
		_fileBuf = E_NEW_ARRAY("filemem", byte, _fileSize);
		_fileBufReserved = _fileSize;
		uint numRead = fread(_fileBuf, 1, _fileSize, fp);

		// close the file.
		fclose(fp);
		fp = NULL;

		// verify the results.
		E_VALIDATE(numRead == _fileSize,
			"filemem", _T("EFileMem::Open fread did not match size."),
		{
			Close();
			return false;
		});
	}

	if (!OnFileOpened())
	{
		Close();
		return false;
	}

	return true;
}


//===================
// EFileMem::OpenMemForReading
//===================
bool		EFileMem::OpenMemForReading(const byte* buf, uint bufSize, uint mode)
{
	// if we're already open, abort.
	E_VERIFY(!IsOpen(), return false);

	// verify the parameters.
	E_VERIFY(buf != NULL && bufSize > 0, return false);

	// verify the mode.
	E_VERIFY(IsModeValid(mode | FILE_READ), return false);

	// open the memory.
	_mode = mode | FILE_READ;
	_fileBuf = BufCpy(_T("filemem"), buf, bufSize);
	_fileSize = bufSize;
	_filePos = 0;
	_growSize = FILE_MEM_GROW_SIZE;
	_ownMem = true;

	if (!OnFileOpened())
	{
		Close();
		return false;
	}

	return true;
}

	
//===================
// EFileMem::OpenMemForWriting
//===================
bool		EFileMem::OpenFixedMemForWriting(byte* buf, uint bufSize, uint mode)
{
	// if we're already open, abort.
	E_VERIFY(!IsOpen(), return false);

	// verify the mode.
	E_VERIFY(IsModeValid(mode | FILE_WRITE), return false);

	// open the memory.
	_mode = mode | FILE_WRITE;
	_fileBuf = buf;
	_fileBufReserved = bufSize;
	_fileSize = bufSize;
	_filePos = 0;
	_growSize = FILE_MEM_GROW_SIZE;
	_ownMem = false;

	if (!OnFileOpened())
	{
		Close();
		return false;
	}

	return true;
}

	
//===================
// EFileMem::OpenMemForWriting
//===================
bool		EFileMem::OpenMemForWriting(uint mode, uint reserveSize)
{
	// if we're already open, abort.
	E_VERIFY(!IsOpen(), return false);

	// verify the mode.
	E_VERIFY(IsModeValid(mode | FILE_WRITE), return false);

	if (reserveSize == 0)
		reserveSize = FILE_MEM_GROW_SIZE;

	// open the memory.
	_mode = mode | FILE_WRITE;
	_fileBuf = E_NEW_ARRAY("filemem", byte, reserveSize);
	_fileBufReserved = reserveSize;
	_filePos = 0;
	_growSize = FILE_MEM_GROW_SIZE;
	_ownMem = true;

	return true;
}


//===================
// EFileMem::Close
//===================
void		EFileMem::Close()
{
	if (_ownMem)
		E_DELETE_ARRAY("filebuf", _fileBuf);
	_fileBufReserved = 0;
	_fileSize = 0;
	_filePos = 0;
	_growSize = FILE_MEM_GROW_SIZE;
	_mode = 0;
	_ownMem = true;
}


//===================
// EFileMem::IsOpen
//===================
bool		EFileMem::IsOpen() const
{
	if (HasMode(FILE_READ))
	{
		if (_fileBuf == NULL)
			return false;
		return true;
	}

	if (HasMode(FILE_WRITE))
		return true;

	return false;
}


//===================
// EFileMem::IsEOF
//===================
bool		EFileMem::IsEOF() const
{
	if (HasMode(FILE_READ) || (!_ownMem && HasMode(FILE_WRITE)))
	{
		if (_fileBuf == NULL)
			return true;

		if (GetPos() >= _fileSize)
			return true;

		return false;
	}

	return true;
}


//===================
// EFileMem::GetPos
//===================
uint		EFileMem::GetPos() const
{
	return _filePos;
}


//===================
// EFileMem::Seek
//===================
bool		EFileMem::Seek(uint pos)
{
	if (HasMode(FILE_READ))
	{
		E_VERIFY(_fileBuf != NULL, return false);
		E_VERIFY(pos <= _fileSize, return false);
		_filePos = pos;
		return true;
	}

	E_VERIFY(HasMode(FILE_WRITE), return false);
	E_VERIFY(pos <= _fileBufReserved, return false);
	_filePos = pos;
	return true;
}


//===================
// EFileMem::Read
//===================
uint		EFileMem::Read(byte* outBuf, uint count)
{
	E_VERIFY(HasMode(FILE_READ), return 0);
	E_VERIFY(_fileBuf != NULL, return 0);
	E_VERIFY(outBuf != NULL && count > 0, return 0);

	uint bytesRead(count);
	uint filePos(GetPos());
	if (filePos + count > _fileSize)
		bytesRead = _fileSize - filePos;

	if (bytesRead > 0)
	{
		MemCpy(outBuf, _fileBuf + _filePos, bytesRead);
		_filePos += bytesRead;
	}
	return bytesRead;
}


//===================
// EFileMem::Write
//===================
uint		EFileMem::Write(const byte* buf, uint bufSize)
{
	E_VERIFY(HasMode(FILE_WRITE), return 0);
	E_VERIFY(buf != NULL, return 0);
	if (bufSize == 0)
		return 0;

	// grow the write buffer.
	uint remainingSize(Grow(GetPos() + bufSize));
	uint writeSize(E_MIN(bufSize, remainingSize));

	// if the grow failed, abort.
	if (_fileBuf == NULL)
		return 0;

	// write into the write buffer.
	MemCpy(_fileBuf + GetPos(), buf, writeSize);
	_filePos += writeSize;

	E_ASSERT(_filePos <= _fileBufReserved);

	return writeSize;
}


//===================
// EFileMem::GetFileSize
//===================
uint		EFileMem::GetFileSize() const
{
	if (HasMode(FILE_READ))
	{
		E_VERIFY(_fileBuf != NULL, return 0);
		return _fileSize;
	}

	E_VERIFY(HasMode(FILE_WRITE), return 0);
	return _fileBufReserved;
}


//===================
// EFileMem::GetFileBuf
//===================
const byte*	EFileMem::GetFileBuf() const
{
	if (HasMode(FILE_READ))
	{
		E_VERIFY(_fileBuf != NULL, return NULL);
		return _fileBuf;
	}

	E_VERIFY(HasMode(FILE_WRITE), return NULL);
	return _fileBuf;
}

