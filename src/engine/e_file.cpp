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
	bool append = (mode & FILE_APPEND) != 0;

	if (read && write)
		return false;

	if (binary && text)
		return false;

	if (append && !write)
		return false;

	return true;
}


//===================
// EFile::OnFileOpened
//===================
bool		EFile::OnFileOpened()
{
	if (HasMode(FILE_READ))
	{
		// for text mode, determine the file encoding.
		_textEncoding = TE_UTF8;
		if (HasMode(FILE_TEXT))
		{
			if (GetFileSize() >= 4)
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

	E_VERIFY(HasMode(FILE_WRITE), return false);

	// for text mode, determine the BOM to write.
	if (HasMode(FILE_TEXT))
	{
		if (HasMode(FILE_APPEND) && GetPos() != 0)
			return true;

		byte bom[4];
		uint bomSize = StrGetEncodingBOM(bom, _textEncoding);
		if (bomSize > 0)
			Write(bom, bomSize);
	}

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


//===================
// EFile::ReadLine
//===================
wstring		EFile::ReadLine()
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
		E_WARN("file", "ReadLine: unsupported file text encoding");
		return WSNULL;
	}

	wstring result;

	E_ASSERT(charSize <= sizeof(uint32));
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
// EFile::GetRemainingSize
//===================
uint		EFile::GetRemainingSize() const
{
	E_VERIFY(IsOpen(), return 0);
	E_VERIFY(HasMode(FILE_READ), return 0);

	uint pos(GetPos());
	uint size(GetFileSize());
	E_VERIFY(pos <= size, return 0);
	return size - pos;
}



