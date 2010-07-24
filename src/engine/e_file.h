//========================================================================
//	file:		e_file.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Constants
//========================================================================
#define FILE_READ			BIT(0)
#define FILE_WRITE			BIT(1)
#define FILE_APPEND			BIT(2)
#define FILE_BINARY			BIT(3)
#define FILE_TEXT			BIT(4)
#define FILE_MEMORY			BIT(5)
//========================================================================

//========================================================================
// Declarations
//========================================================================
struct SVec3;
class MVec3;
class MMat33;
//========================================================================

//========================================================================
// EFile
//========================================================================
class EFile
{
protected:
	wstring			_fullPath;
	wstring			_name;
	wstring			_extension;
	string			_textHeaderPrefix;
	string			_textHeaderPadding;
	string			_textHeaderStr;
	uint			_mode;
	uint			_indent;
	bool			_indentHeaderLines;
	bool			_writingTextHeader;
	ETextEncoding	_textEncoding;

	// check for conflicting mode flags.
	bool				IsModeValid(uint mode) const;

	bool				OnFileOpened();
	void				OnFileClosed();

	template<class T>
	uint				ReadType(T& val)
	{
		return Read((byte*)&val, sizeof(T));
	}

	uint				PrefixLine(uint numLineChars, uint indent);

public:
	// ctor & dtor.
	EFile();
	virtual ~EFile() { }

	const wstring&		GetFullPath() const			{ return _fullPath; }
	const wstring&		GetName() const				{ return _name; }
	const wstring&		GetExtension() const		{ return _extension; }

	uint				GetMode() const				{ return _mode; }
	bool				HasMode(uint flag) const	{ return (_mode & flag) != 0; }

	// per-file methods.
	virtual bool		Open(const wstring& path, uint mode = FILE_READ);
	virtual void		Close()=0;
	virtual bool		IsOpen() const=0;
	virtual bool		IsEOF() const=0;

	virtual uint		GetPos() const=0;
	virtual bool		Seek(uint pos)=0;

	virtual uint		Read(byte* outBuf, uint count)=0;
	virtual wstring		ReadLine();

	template<class T>
	bool				ReadArray(T* vals, uint count)
	{
		E_VERIFY(vals != NULL && count > 0, return false);

		do 
		{
			if (!ReadType(*vals++))
				return false;
		} while (--count != 0);

		return true;
	}

	// returns number of bytes written.
	virtual uint		Write(const byte* buf, uint bufSize)=0;

	// writes a string to the file in UTF-8 without BOM.
	// returns the number of bytes written.
	uint				WriteText(const wstring& str);
	uint				WriteText(const string& str);
	uint				WriteText(const char* str, uint count);

	// sets the indentation level for all WriteLine, WriteBlankLine, etc functions.
	void				SetIndent(uint indent)					{ _indent = indent; }

	// writes an indented string followed by a newline.
	// returns the number of bytes written.
	uint				WriteLine(const wstring& str);
	uint				WriteLine(const string& str);

	// returns the number of bytes written.
	uint				WriteBlankLine(uint count = 1);
	uint				WriteBlankLines(uint count);


	// writes a header string.
	// the result of calling
	//		WriteLine("Model 'baz'");
	//		WriteLine("(child of 'bar')");
	//		WriteLine("(child of 'foo')");
	// will be:
	//		//===================
	//		// Model 'baz'
	//		//   (child of 'bar')
	//		//   (child of 'baz')
	//		//===================
	uint				BeginTextHeader(const string& headerStr = "===================", const string& headerPrefix = "//");
	uint				EndTextHeader();


	virtual uint		GetFileSize() const=0;
	uint				GetRemainingSize() const;
	const byte*			GetFileBuf() const				{ return NULL; }

	// general I/O methods.
	byte				ReadByte();
	short				ReadShort();
	int					ReadInt();
	float				ReadFloat();
	SVec3				ReadSVec3();
	MVec3				ReadMVec3();
	MMat33				ReadMMat33(bool rowMajor = true);
	MTransform			ReadTransform(bool rowMajor = true);
};
//========================================================================

