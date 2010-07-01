//========================================================================
//	file:		e_filedisk.cpp
//	author:		Shawn Presser 
//	date:		6/30/10
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
	// if we're already open, abort.
	if (_fp != NULL)
		return false;

	// validate the mode.
	if (!IsModeValid(mode))
		return false;

	// store the file mode.
	_mode = mode;

	// open the file.
	if (HasMode(FILE_READ))
		_fp = _wfopen(path.c_str(), _T("rb"));
	else if (HasMode(FILE_WRITE))
		if (HasMode(FILE_APPEND))
			_fp = _wfopen(path.c_str(), _T("wb+"));
		else
			_fp = _wfopen(path.c_str(), _T("wb"));

	if (_fp == NULL)
		return false;

	// determine the file size.
	fseek(_fp, 0, SEEK_END);
	_fileSize = (uint)ftell(_fp);
	fseek(_fp, 0, SEEK_SET);

	// for text mode, determine the file encoding.
	_textEncoding = TE_UTF8;
	if (HasMode(FILE_TEXT))
	{
		if (_fileSize >= 4)
		{
			// read the byte-order mark.
			byte bom[4];
			uint bomSize(Read(bom, 4));
			uint fileBomSize(0);
			_textEncoding = StrClassifyEncoding(fileBomSize, bom, bomSize);

			// skip over the byte-order mark.
			Seek(fileBomSize);
		}
	}

	return true;
}


//===================
// EFileDisk::Close
//===================
void		EFileDisk::Close()
{
	if (_fp == NULL)
		return;

	fclose(_fp);
	_fp = NULL;
	_fileSize = 0;
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
	if (_fp == NULL)
		return true;

	if (GetPos() >= _fileSize)
		return true;
	return false;
}


//===================
// EFileDisk::GetPos
//===================
uint		EFileDisk::GetPos() const
{
	if (_fp == NULL)
		return 0;

	uint pos((uint)ftell(_fp));
	return pos;
}


//===================
// EFileDisk::Seek
//===================
void		EFileDisk::Seek(uint pos)
{
	if (_fp == NULL)
		return;

	fseek(_fp, pos, SEEK_SET);
}


//===================
// EFileDisk::Read
//===================
uint		EFileDisk::Read(byte* outBuf, uint count)
{
	if (_fp == NULL)
		return 0;

	if (!(_mode & FILE_READ))
		return 0;

	int bytesRead(fread(outBuf, 1, count, _fp));
	if (bytesRead < 0)
		return 0;

	return bytesRead;
}


//===================
// EFileDisk::ReadLine
//===================
wstring		EFileDisk::ReadLine()
{
	E_VERIFY(HasMode(FILE_TEXT), return WSNULL);

	// determine how many bytes need to be read per character.
	uint charSize(0);
	switch (_textEncoding)
	{
	case TE_UTF8:
	case TE_UTF8_WITH_BOM:
		charSize = 1;
		break;

	case TE_UTF16:
	case TE_UTF16_LITTLE_ENDIAN:
		charSize = 2;
		break;
	}

	if (charSize == 0)
	{
		assert(!"Unsupported file text encoding.");
		return WSNULL;
	}

	wstring result;

	assert(charSize <= sizeof(uint32));
	uint32 buf;
	while (!IsEOF())
	{
		buf = 0;
		uint bytesRead = Read((byte*)&buf, charSize);
		if (bytesRead != charSize)
			return result;

		wchar_t c(buf);
		if (c == _T('\r'))
		{
			uint prevPos(GetPos());
			buf = 0;
			bytesRead = Read((byte*)&buf, charSize);
			if (bytesRead == charSize)
			{
				wchar_t c2(buf);
				if (c2 == '\n')
					break;
			}
			Seek(prevPos);
		}

		if (c == _T('\n'))
			break;

		result.append(1, c);
	}

	return result;
}


//===================
// EFileDisk::Write
//===================
uint		EFileDisk::Write(const byte* buf, uint bufSize)
{
	return 0;
}


//===================
// EFileDisk::WriteLine
//===================
uint		EFileDisk::WriteLine(const wstring& str)
{
	return 0;
}


//===================
// EFileDisk::WriteLine
//===================
uint		EFileDisk::WriteLine(const string& str)
{
	return 0;
}


//===================
// EFileDisk::GetFileSize
//===================
uint		EFileDisk::GetFileSize() const
{
	return 0;
}


//===================
// EFileDisk::GetFileBuffer
//===================
const byte*	EFileDisk::GetFileBuffer() const
{
	return NULL;
}

