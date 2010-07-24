//========================================================================
//	file:		e_ref.h
//	author:		Shawn Presser 
//	date:		7/17/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

//========================================================================
// Declarations
//========================================================================
//========================================================================

//========================================================================
// ERefCounted
//========================================================================
class ENGINE_API ERefCounted
{
private:
	uint		_refCount;

public:
	//===================
	// ERefCounted::ERefCounted
	//===================
	ERefCounted()
		: _refCount(0)
	{
	}


	//===================
	// ERefCounted::~ERefCounted
	//===================
	virtual ~ERefCounted()
	{
	}


	//===================
	// ERefCounted::IncRef
	//===================
	void		IncRef()
	{
		++_refCount;
	}


	//===================
	// ERefCounted::Release
	//===================
	void		Release()
	{
		E_VERIFY(_refCount > 0, return);

		--_refCount;

		if (_refCount == 0)
		{
			E_DELETE_SELF("ref", this);
		}
	}
};
//========================================================================

//========================================================================
// ERef
//========================================================================
template< typename T >
class ERef
{
private:
	T*			_val;

public:
	//===================
	// ERef::ERef
	//===================
	ERef()
		: _val(NULL)
	{
	}


	//===================
	// ERef::ERef(val)
	//===================
	ERef(T* val)
		: _val(val)
	{
		if (_val)
			_val->IncRef();
	}


	//===================
	// ERef::ERef(val)
	//===================
	ERef(const T* const val)
		: _val(const_cast<T*>(val))
	{
		if (_val)
			_val->IncRef();
	}


	//===================
	// ERef::ERef(val)
	//===================
	ERef(const ERef<T>& val)
		: _val(const_cast<T*>(val.Get()))
	{
		if (_val)
			_val->IncRef();
	}


	//===================
	// ERef::~ERef
	//===================
	~ERef()
	{
		if (_val)
			_val->Release();
	}


	//===================
	// ERef::Get
	//===================
	const T*				Get() const			{ return _val; }
	T*						Get()				{ return _val; }


	//===================
	// ERef::Reset
	//===================
	void					Reset(T* val = NULL)
	{
		if (_val != NULL)
			_val->Release();

		if (val != NULL)
			val->IncRef();

		_val = val;
	}


	//===================
	// ERef::operator =
	//===================
	ERef&					operator =(const ERef& rhs)
	{
		Reset(const_cast<T*>(rhs.Get()));
		return *this;
	}


	//===================
	// ERef::operator bool
	//===================
	operator				bool() const
	{
		if (_val == NULL)
			return false;

		return true;
	}


	//===================
	// ERef::operator T*
	//===================
	operator				const T*() const	{ return _val; }
	operator				T*()				{ return _val; }


	//===================
	// ERef::operator ->
	//===================
	const T*				operator ->() const	{ return _val; }
	T*						operator ->()		{ return _val; }
};
//========================================================================
