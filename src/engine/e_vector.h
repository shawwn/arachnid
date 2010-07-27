//========================================================================
//	file:		e_vector.h
//	author:		Shawn Presser 
//	date:		7/23/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

#include "e_vector_policies.h"

//========================================================================
// EVector
//========================================================================
template< typename T, typename OnCreate, typename OnDestroy, typename OnAssign >
class ENGINE_API EVector
{
protected:
	typedef typename EVector< T, OnCreate, OnDestroy, OnAssign >		EVector_t;

	T*						_items;
	uint					_growBy;
	uint					_count;
	uint					_reserved;


	inline void				reset(const EVector_t* rhs = NULL)
	{
		if (rhs == NULL)
		{
			OnDestroy::Destroy(_items, _count);
			_count = 0;
			return;
		}

		_growBy = rhs->_growBy;
		assign(rhs->ptr(), rhs->_count);
	}

public:

	typedef	typename T			value_t;
	typedef typename OnCreate	OnCreate_t;
	typedef typename OnDestroy	OnDestroy_t;
	typedef typename OnAssign	OnAssign_t;


	EVector(uint growBy /* probably choose a power of 2 */)
		: _items(NULL)
		, _growBy(growBy < 1 ? 1 : growBy)
		, _count(0)
		, _reserved(0)
	{
	}


	EVector(const EVector& rhs)
		: _items(NULL)
		, _growBy(rhs._growBy)
		, _count(0)
		, _reserved(0)
	{
		reset(&rhs);
	}


	~EVector()
	{
		reset();
		MemFree(_items);
	}


	inline void			reserve(uint num)
	{
		if (num <= _reserved)
			return;

		_reserved = _growBy * (num / _growBy);
		if (num % _growBy > 0)
			_reserved += _growBy;

		_items = (T*)MemRealloc((byte*)_items, sizeof(T) * _reserved);
	}


	inline void			reserve_exact(uint num)
	{
		if (num <= _reserved)
			return;

		_reserved = num;
		_items = (T*)MemRealloc((byte*)_items, sizeof(T) * num);
	}


	inline void			resize(uint size)
	{
		if (size == _count)
			return;

		if (size < _count)
		{
			pop_back(size - _count);
			return;
		}

		reserve_exact(size);
		OnCreate::Create(_items + _count, size - _count);
		_count = size;
	}


	inline void			resize(uint size, const T& defaultItem)
	{
		if (size == _count)
			return;

		if (size < _count)
		{
			pop_back(size - _count);
			return;
		}

		reserve_exact(size);
		while (_count < size)
		{
			OnAssign::CreateFrom(_items + _count, &defaultItem, 1);
			++_count;
		}
	}


	inline void			assign(const T* items, uint numItems)
	{
		reserve_exact(numItems);

		if (numItems <= _count)
		{
			OnAssign::Assign(_items, items, numItems);
			OnDestroy::Destroy(_items + numItems, _count - numItems);
		}
		else
		{
			OnAssign::Assign(_items, items, _count);
			OnAssign::CreateFrom(_items + _count, items + _count, numItems - _count);
		}
		_count = numItems;
	}


	inline uint			size() const
	{
		return _count;
	}


	inline void			clear()
	{
		reset();
	}


	inline void			push_back(const T& item)
	{
		reserve(_count + 1);
		OnAssign::CreateFrom(_items + _count, &item, 1);
		++_count;
	}


	inline void			pop_back(uint count = 1)
	{
		T* at(_items + _count);
		if (count >= _count)
		{
			at = _items;
			count = _count;
		}
		else
		{
			at -= count;
		}

		E_ASSERT(count <= _count);
		OnDestroy::Destroy(at, count);
		_count -= count;
	}


	inline void			erase_swap(uint pos, uint count = 1)
	{
		E_VERIFY(pos + count <= _count, return);

		uint at(pos);
		uint last(_count - 1); 
		uint with(last);

		for (count; count > 0 && at < with; --count)
			SwapRaw(_items[at++], _items[with--]);

		uint numRemoved(last - with);
		if (at == with)
			++numRemoved;

		pop_back(numRemoved);
	}


	T*					ptr()					{ return _items; }
	const T*			ptr() const				{ return _items; }


	T&					operator [](uint idx)
	{
		E_ASSERT(idx < _count);
		return _items[idx];
	}


	const T&			operator [](uint idx) const
	{
		E_ASSERT(idx < _count);
		return _items[idx];
	}


	EVector_t&			operator =(const EVector_t& rhs)
	{
		reset(&rhs);
		return *this;
	}
};
//========================================================================

// a vector containing objects.
BuildVectorTemplate(EVecObj,
				StoragePolicy::OnCreate::ConstructObj,
				StoragePolicy::OnDestroy::DestructObj,
				StoragePolicy::OnAssign::CopyObj);

// a vector containing uninitialized entries.
BuildVectorTemplate(EVecRawObj,
				StoragePolicy::OnCreate::DoNothing,
				StoragePolicy::OnDestroy::DoNothing,
				StoragePolicy::OnAssign::CopyObj);

// a vector containing objects who are initialized to zero on creation, and whose
// destructor is called.
BuildVectorTemplate(EVecZeroObj,
				StoragePolicy::OnCreate::ZeroMem,
				StoragePolicy::OnDestroy::DestructObj,
				StoragePolicy::OnAssign::CopyObj);

// a vector containing raw uninitialized memory and copied byte-for-byte.
BuildVectorTemplate(EVecRawMem,
				StoragePolicy::OnCreate::ZeroMem,
				StoragePolicy::OnDestroy::DoNothing,
				StoragePolicy::OnAssign::CopyRawMemory);

// a vector containing raw memory initialized to zero on creation and copied byte-for-byte.
BuildVectorTemplate(EVecZeroMem,
				StoragePolicy::OnCreate::ZeroMem,
				StoragePolicy::OnDestroy::DoNothing,
				StoragePolicy::OnAssign::CopyRawMemory);
