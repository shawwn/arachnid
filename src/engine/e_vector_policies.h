//========================================================================
//	file:		e_vector_policies.h
//	author:		Shawn Presser 
//	date:		7/23/10
//
// (c) 2010 Shawn Presser.  All Rights Reserved.
//========================================================================
#pragma once

namespace StoragePolicy
{
	namespace OnCreate
	{
		struct DoNothing
		{
			template< typename T >
			inline static void		Create(T* dst, uint count) { }
		};

		struct ZeroMem
		{
			template< typename T >
			inline static void		Create(T* dst, uint count)
			{
				MemZero((byte*)dst, sizeof(T) * count);
			}
		};

		struct ConstructObj
		{
			template< typename T >
			inline static void		Create(T* dst, uint count)
			{
				while (count > 0)
				{
					--count;
					new (&dst) T();
				}
			}
		};
	};

	namespace OnDestroy
	{
		struct DoNothing
		{
			template< typename T >
			inline static void		Destroy(T* dst, uint count)
			{
			}
		};

		struct DestructObj
		{
			template< typename T >
			inline static void		Destroy(T* dst, uint count)
			{
				while (count > 0)
				{
					--count;
					(*dst++).~T();
				}
			}
		};
	};

	namespace OnAssign
	{
		struct CopyObj
		{
			template< typename T >
			inline static void		Assign(T* dst, const T* src, uint count)
			{
				E_ASSERT(count == 0 || dst != src);
				while (count > 0)
				{
					--count;
					*dst++ = *src++;
				}
			}

			template< typename T >
			inline static void		CreateFrom(T* dst, const T* src, uint count)
			{
				E_ASSERT(count == 0 || dst != src);
				while (count > 0)
				{
					--count;
					new (dst++) T(*src++);
				}
			}
		};

		struct CopyRawMemory
		{
			template< typename T >
			inline static void		Assign(T* dst, const T* src, uint count)
			{
				E_ASSERT(count == 0 || dst != src);
				MemCpy(dst, src, sizeof(T) * count);
			}

			template< typename T >
			inline static void		CreateFrom(T* dst, const T* src, uint count)
			{
				E_ASSERT(count == 0 || dst != src);
				MemCpy(dst, src, sizeof(T) * count);
			}
		};
	}
};

//========================================================================
// Macros
//========================================================================
#define BuildVectorTemplate(name, onCreate, onDestroy, onAssign)				\
template< typename T >															\
class ENGINE_API name : public EVector< T, onCreate, onDestroy, onAssign >		\
{																				\
	typedef typename EVector< T, onCreate, onDestroy, onAssign >	vec_t;		\
public:																			\
	name(uint growBy /* probably choose a power of 2 */)						\
	: vec_t(growBy)																\
	{																			\
	}																			\
																				\
	name(const name& rhs)														\
	: vec_t(rhs)																\
	{																			\
	}																			\
};

#define BuildVectorType(name, T, growByDefault, onCreate, onDestroy, onAssign)	\
class ENGINE_API name : public EVector< T, onCreate, onDestroy, onAssign >		\
{																				\
	typedef EVector< T, onCreate, onDestroy, onAssign >	vec_t;					\
public:																			\
	name(uint growBy = growByDefault)											\
	: vec_t(growBy)																\
	{																			\
	}																			\
																				\
	name(const name& rhs)														\
	: vec_t(rhs)																\
	{																			\
	}																			\
};

#define BuildVectorTypeFrom(name, T, growByDefault, from)						\
class ENGINE_API name : public EVector< T,	typename from<T>::OnCreate_t,		\
											typename from<T>::OnDestroy_t,		\
											typename from<T>::OnAssign_t >		\
{																				\
	typedef EVector< T,				typename from<T>::OnCreate_t,				\
									typename from<T>::OnDestroy_t,				\
									typename from<T>::OnAssign_t >	vec_t;		\
public:																			\
	name(uint growBy = growByDefault)											\
	: vec_t(growBy)																\
	{																			\
	}																			\
																				\
	name(const name& rhs)														\
	: vec_t(rhs)																\
	{																			\
	}																			\
};
//========================================================================
