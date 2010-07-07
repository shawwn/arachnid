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
#define FILE_MEMORY			BIT(5)
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

	bool				OnFileOpened();

public:
	// ctor & dtor.
	EFile();
	virtual ~EFile() { }

	uint				GetMode() const				{ return _mode; }
	bool				HasMode(uint flag) const	{ return (_mode & flag) != 0; }

	// per-file methods.
	virtual bool		Open(const wstring& path, uint mode = FILE_READ)=0;
	virtual void		Close()=0;
	virtual bool		IsOpen() const=0;
	virtual bool		IsEOF() const=0;

	virtual uint		GetPos() const=0;
	virtual bool		Seek(uint pos)=0;

	virtual uint		Read(byte* outBuf, uint count)=0;
	virtual wstring		ReadLine();

	template<class T>
	bool				ReadType(T& val)
	{
		if (Read((byte*)&val, sizeof(T)) == sizeof(T))
			return true;
		return false;
	}

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

	virtual uint		Write(const byte* buf, uint bufSize)=0;
	virtual uint		WriteLine(const wstring& str)	{ return 0; }
	virtual uint		WriteLine(const string& str)	{ return 0; }

	virtual uint		GetFileSize() const=0;
	virtual uint		GetRemainingSize() const;
	virtual const byte*	GetFileBuf() const				{ return NULL; }

	// general I/O methods.

	byte				ReadByte()
	{
		byte r(0);
		uint bytesRead(Read((byte*)&r, 1));
		if (bytesRead != 1)
			E_WARN("file", "ReadByte: hit end of file");
		return r;
	}

	int					ReadInt()
	{
		int r(0);
		uint bytesRead(Read((byte*)&r, 4));
		if (bytesRead != 4)
			E_WARN("file", "ReadInt: hit end of file");
		return r;
	}

	float				ReadNum()
	{
		float r(0.0f);
		uint bytesRead(Read((byte*)&r, 4));
		if (bytesRead != 4)
			E_WARN("file", "ReadFloat: hit end of file");
		return r;
	}
};
//========================================================================

