//========================================================================
//	file:		e_exception.h
//	author:		Shawn Presser 
//	date:		7/1/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// EException
//========================================================================
class EException : public exception
{
private:
	wstring			_context;
	wstring			_msg;
public:
	EException(const wstring& msg)
		: exception(WStringToString(msg).c_str())
		, _msg(msg)
	{
	}

	EException(const wstring& context, const wstring& msg)
		: exception(WStringToString(msg).c_str())
		, _context(context)
		, _msg(msg)
	{
	}

	const wstring&			GetContext() const		{ return _context; }
	const wstring&			GetMessage() const		{ return _msg; }
};
//========================================================================