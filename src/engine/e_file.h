//========================================================================
//	file:		e_file.h
//	author:		Shawn Presser 
//	date:		6/30/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
#define FILE_READ			BIT(0)
#define FILE_WRITE			BIT(1)
#define FILE_APPEND			BIT(2)
#define FILE_BINARY			BIT(3)
#define FILE_TEXT			BIT(4)
//========================================================================

//========================================================================
// EFile
//========================================================================
class EFile
{
protected:
	uint			_mode;
	ETextEncoding	_textEncoding;

	// check for conflicting mode flags.
	bool				IsModeValid(uint mode) const;

public:
	// ctor & dtor.
	EFile();
	virtual ~EFile() { }

	uint				GetMode() const		{ return _mode; }
	bool				HasMode(uint flag)	{ return (_mode & flag) != 0; }

	// per-file methods.
	virtual bool		Open(const wstring& path, uint mode = FILE_READ)=0;
	virtual void		Close()=0;
	virtual bool		IsOpen() const=0;
	virtual bool		IsEOF() const=0;

	virtual uint		GetPos() const=0;
	virtual void		Seek(uint pos)=0;

	virtual uint		Read(byte* outBuf, uint count)=0;
	virtual wstring		ReadLine()=0;

	virtual uint		Write(const byte* buf, uint bufSize)=0;
	virtual uint		WriteLine(const wstring& str)=0;
	virtual uint		WriteLine(const string& str)=0;

	virtual uint		GetFileSize() const=0;
	virtual const byte*	GetFileBuffer() const=0;
};
//========================================================================

