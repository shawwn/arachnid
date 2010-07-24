//========================================================================
//	file:		e_file.cpp
//	author:		Shawn Presser 
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================

//========================================================================
// Headers
//========================================================================
#include "e_common.h"
#include "e_file.h"

// engine headers.
#include "e_filemanager.h"

// graphics headers.
#include "gr_types.h"

// math headers.
#include "m_vec3.h"
#include "m_mat33.h"
#include "m_mat44.h"
#include "m_transform.h"
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
// EFile::OnFileOpened
//===================
void				EFile::OnFileClosed()
{
	_mode = 0;
	_indent = 0;
	_textEncoding = TE_UTF8;

	E_ASSERT(!_writingTextHeader);
	_textHeaderPrefix.clear();
	_textHeaderStr.clear();
	_writingTextHeader = false;
	_indentHeaderLines = false;
}


//===================
// EFile::PrefixLine
//===================
uint				EFile::PrefixLine(uint numLineChars, uint indent)
{
	uint bytes(0);

	if (_writingTextHeader)
	{
		// write the indentation.
		while (indent != 0)
		{
			bytes += WriteText(E_INDENT, E_INDENT_CHARS);
			--indent;
		}

		// write the header prefix.
		bytes += WriteText(_textHeaderPrefix);

		// write the padding.
		bytes += WriteText(_textHeaderPadding);

		// write the header indentation, if necessary.
		if (_indentHeaderLines && numLineChars > 0)
			bytes += WriteText("    ", 2);

		// indent subsequent header lines.
		_indentHeaderLines = true;
	}
	else
	{
		// if we're writing a blank line, then don't indent.
		if (numLineChars > 0)
		{
			// write the indentation.
			while (indent != 0)
			{
				bytes += WriteText(E_INDENT, E_INDENT_CHARS);
				--indent;
			}
		}
	}

	return bytes;
}


//===================
// EFile::EFile
//===================
EFile::EFile()
: _mode(0)
, _indent(0)
, _textEncoding(TE_UTF8)
, _writingTextHeader(false)
, _indentHeaderLines(false)
{
}


//===================
// EFile::Open
//===================
bool		EFile::Open(const wstring& path, uint mode)
{
	E_VERIFY(gFileManager != NULL, return false);

	// if we're already open, abort.
	E_VERIFY(!IsOpen(), return false);

	// validate the mode.
	E_VERIFY(IsModeValid(mode), return false);

	// store the file mode.
	_mode = mode;

	// store the file path.
	_fullPath = path;
	gFileManager->ParsePath(_fullPath, _name, _extension);

	return true;
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
// EFile::WriteText
//===================
uint				EFile::WriteText(const wstring& str)
{
	// write UTF-8.
	return WriteText(WStringToString(str));
}


//===================
// EFile::WriteText
//===================
uint				EFile::WriteText(const string& str)
{
	if (str.empty())
		return 0;

	E_ASSERT(_textEncoding == TE_UTF8);
	return Write((const byte*)str.c_str(), sizeof(char) * str.size());
}


//===================
// EFile::WriteText
//===================
uint				EFile::WriteText(const char* str, uint count)
{
	if (count == 0)
		return 0;

	return Write((const byte*)str, sizeof(char) * count);
}


//===================
// EFile::WriteLine
//===================
uint				EFile::WriteLine(const wstring& str)
{
	uint bytes(0);

	// write the line prefix.
	bytes += PrefixLine((uint)str.size(), _indent);

	// write the string.
	bytes += WriteText(str);

	// write the newline.
	bytes += WriteText(E_NEWLINE, E_NEWLINE_CHARS);

	// return number of bytes written.
	return bytes;
}


//===================
// EFile::WriteLine
//===================
uint				EFile::WriteLine(const string& str)
{
	uint bytes(0);

	// write the line prefix.
	bytes += PrefixLine((uint)str.size(), _indent);

	// write the string.
	bytes += WriteText(str);

	// write the newline.
	bytes += WriteText(E_NEWLINE, E_NEWLINE_CHARS);

	// return number of bytes written.
	return bytes;
}


//===================
// EFile::WriteBlankLine
//===================
uint				EFile::WriteBlankLine(uint count)
{
	uint bytes(0);

	while (count > 0)
	{
		// write the line prefix.
		PrefixLine(0, _indent);

		// write the newline.
		bytes += WriteText(E_NEWLINE, E_NEWLINE_CHARS);

		// write the next blank line.
		--count;
	}

	// return number of bytes written.
	return bytes;
}


//===================
// EFile::BeginTextHeader
//===================
uint				EFile::BeginTextHeader(const string& headerStr, const string& headerPrefix)
{
	uint bytes(0);

	E_VERIFY(!_writingTextHeader, return 0);
	_writingTextHeader = true;
	_indentHeaderLines = false;

	// store the text header info.
	_textHeaderPrefix = headerPrefix;
	_textHeaderStr = headerStr;

	// write the header line.
	bytes += WriteLine(headerStr);

	// the header line doesn't count.
	_indentHeaderLines = false;

	// add padding.
	_textHeaderPadding = " ";

	return bytes;
}


//===================
// EFile::EndTextHeader
//===================
uint				EFile::EndTextHeader()
{
	uint bytes(0);

	E_VERIFY(_writingTextHeader, return 0);

	// remove padding.
	_textHeaderPadding.clear();

	// don't indent the footer line.
	_indentHeaderLines = false;

	// write the footer line.
	bytes += WriteLine(_textHeaderStr);

	// we are no longer writing the text header.
	_writingTextHeader = false;

	return bytes;
}


//===================
// EFile::GetRemainingSize
//===================
uint				EFile::GetRemainingSize() const
{
	E_VERIFY(IsOpen(), return 0);
	E_VERIFY(HasMode(FILE_READ), return 0);

	uint pos(GetPos());
	uint size(GetFileSize());
	E_VERIFY(pos <= size, return 0);
	return size - pos;
}


//===================
// EFile::ReadByte
//===================
byte				EFile::ReadByte()
{
	byte r(0);
	uint bytesRead(ReadType(r));
	E_VALIDATE(bytesRead == sizeof(byte),
		"file", _T("ReadByte: hit end of file"),
		return 0);
	return r;
}


//===================
// EFile::ReadShort
//===================
short				EFile::ReadShort()
{
	short r(0);
	uint bytesRead(ReadType(r));
	E_VALIDATE(bytesRead == sizeof(short),
		"file", _T("ReadShort: hit end of file"),
		return 0);
	return r;
}


//===================
// EFile::ReadInt
//===================
int					EFile::ReadInt()
{
	int r(0);
	uint bytesRead(ReadType(r));
	E_VALIDATE(bytesRead == sizeof(int),
		"file", _T("ReadInt: hit end of file"),
		return 0);
	return r;
}


//===================
// EFile::ReadFloat
//===================
float				EFile::ReadFloat()
{
	float r(0.0f);
	uint bytesRead(ReadType(r));
	E_VALIDATE(bytesRead == sizeof(float),
		"file", _T("ReadFloat: hit end of file"),
		return 0.0f);
	return r;
}


//===================
// EFile::ReadSVec3
//===================
SVec3				EFile::ReadSVec3()
{
	SVec3 r;
	uint bytesRead(0);

	bytesRead += ReadType(r.X());
	bytesRead += ReadType(r.Y());
	bytesRead += ReadType(r.Z());
	E_VALIDATE(bytesRead == 3*sizeof(float),
		"file", _T("ReadSVec3: hit end of file"),
		return SVec3::Zero);
	return r;
}


//===================
// EFile::ReadMVec3
//===================
MVec3				EFile::ReadMVec3()
{
	MVec3 r;
	uint bytesRead(0);

	bytesRead += ReadType(r.X());
	bytesRead += ReadType(r.Y());
	bytesRead += ReadType(r.Z());
	E_VALIDATE(bytesRead == 3*sizeof(float),
		"file", _T("ReadMVec3: hit end of file"),
		return MVec3::Zero);
	return r;
}


//===================
// EFile::ReadMMat33
//===================
MMat33				EFile::ReadMMat33(bool rowMajor)
{
	uint bytesRead(0);
	float vals[3][3];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			bytesRead += ReadType(vals[i][j]);

	E_VALIDATE(bytesRead == 9*sizeof(float),
		"file", _T("ReadMMat33: hit end of file"),
		return MMat33::Identity);

	MMat33 r;
	if (rowMajor)
	{
		for (int row = 0; row < 3; ++row)
			for (int col = 0; col < 3; ++col)
				r(row, col) = vals[row][col];
	}
	else
	{
		for (int row = 0; row < 3; ++row)
			for (int col = 0; col < 3; ++col)
				r(row, col) = vals[col][row];
	}
	return r;
}


//===================
// EFile::ReadMTransform
//===================
MTransform			EFile::ReadTransform(bool rowMajor)
{
	uint bytesRead(0);
	float vals[4][3];
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 3; ++j)
			bytesRead += ReadType(vals[i][j]);

	E_VALIDATE(bytesRead == 12*sizeof(float),
		"file", _T("ReadTransform: hit end of file"),
		return MTransform::Identity);

	MMat44 r;
	if (rowMajor)
	{
		for (int row = 0; row < 4; ++row)
			for (int col = 0; col < 3; ++col)
				r(row, col) = vals[row][col];
	}
	else
	{
		for (int row = 0; row < 3; ++row)
			for (int col = 0; col < 4; ++col)
				r(row, col) = vals[col][row];
	}
	return MTransform(r);
}



