#ifndef CC_MEM_DEF_H_
#define CC_MEM_DEF_H_

#include "AllocatedObj.h"
#include "StdAlloc.h"
#include "NedPooling.h"
#include "JeAlloc.h"

CC_NAMESPACE_BEGIN

#if (CC_MEMORY_ALLOCATOR == CC_MEMORY_ALLOCATOR_STD)

// configure default allocators based on the options above
// notice how we're not using the memory categories here but still roughing them out
// in your allocators you might choose to create different policies per category

// configurable category, for general malloc
// notice how we ignore the category here, you could specialize
class CategorisedAllocPolicy : public StdAllocPolicy {};

#elif (CC_MEMORY_ALLOCATOR == CC_MEMORY_ALLOCATOR_NEDPOOLING)

// configure default allocators based on the options above
// notice how we're not using the memory categories here but still roughing them out
// in your allocators you might choose to create different policies per category

// configurable category, for general malloc
// notice how we ignore the category here, you could specialize
class CategorisedAllocPolicy : public NedPoolingAllocPolicy {};

#elif (CC_MEMORY_ALLOCATOR == CC_MEMORY_ALLOCATOR_JEMALLOC)

class CategorisedAllocPolicy : public JeAllocPolicy {};

#endif


// Useful shortcuts
typedef CategorisedAllocPolicy GAP;
typedef CategorisedAllocPolicy STLAP;

// Containers (by-value only)
// Will  be of the form:
// typedef STLAllocator<T, DefaultAllocPolicy, Category> TAlloc;
// for use in vector<T, TAlloc>::type 

//////////////////////////////////////////////////////////////////////////

/** Utility function for constructing an array of objects with placement new,
	without using new[] (which allocates an undocumented amount of extra memory
	and so isn't appropriate for custom allocators).
*/
template<typename T>
T* ConstructN(T* basePtr, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		new ((void*)(basePtr+i)) T();
	}
	return basePtr;
}

CC_NAMESPACE_END

#ifdef CC_MEMORY_TRACKER

/// Allocate a block of raw memory.
#	define _CC_MALLOC(bytes) ::cc::CategorisedAllocPolicy::AllocateBytes(bytes, __FILE__, __LINE__, __FUNCTION__)
/// Reallocate a block of raw memory.
#	define _CC_REALLOC(ptr, bytes) ::cc::CategorisedAllocPolicy::ReallocateBytes(ptr, bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type.
#	define _CC_ALLOC_T(T, count) static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with _CC_MALLOC or _CC_ALLOC_T.
#	define _CC_FREE(ptr) ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define _CC_NEW_T(T) new (::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T(T, count) ::cc::ConstructN(static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count)
/// Free the memory allocated with _CC_NEW_T.
#	define _CC_DELETE_T(ptr, T) if(ptr){(ptr)->~T(); ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr);}
/// Free the memory allocated with _CC_NEW_ARRAY_T.
#	define _CC_DELETE_ARRAY_T(ptr, T, count) if(ptr){for (size_t b = 0; b < (size_t)count; ++b) {(ptr)[b].~T();} ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to user defined boundaries.
#	define _CC_MALLOC_ALIGN(bytes, align) ::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, bytes, __FILE__, __LINE__, __FUNCTION__)
/// Allocate a block of memory for a primitive type aligned to user defined boundaries.
#	define _CC_ALLOC_T_ALIGN(T, count, align) static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__))
/// Free the memory allocated with either _CC_MALLOC_ALIGN or _CC_ALLOC_T_ALIGN.
#	define _CC_FREE_ALIGN(ptr, align) ::cc::CategorisedAllocPolicy::DeallocateBytesAligned(ptr)
/// Allocate a block of raw memory aligned to SIMD boundaries.
#	define _CC_MALLOC_SIMD(bytes) _CC_MALLOC_ALIGN(bytes, CC_SIMD_ALIGNMENT)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries.
#	define _CC_ALLOC_T_SIMD(T, count) _CC_ALLOC_T_ALIGN(T, count, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with either _CC_MALLOC_SIMD or _CC_ALLOC_T_SIMD.
#	define _CC_FREE_SIMD(ptr) _CC_FREE_ALIGN(ptr, CC_SIMD_ALIGNMENT)

/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define _CC_NEW_T_ALIGN(T, align) new (::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T), __FILE__, __LINE__, __FUNCTION__)) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T_ALIGN(T, count, align) ::cc::ConstructN(static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T)*(count), __FILE__, __LINE__, __FUNCTION__)), count)
/// Free the memory allocated with _CC_NEW_T_ALIGN.
#	define _CC_DELETE_T_ALIGN(ptr, T, align) if(ptr){(ptr)->~T(); ::cc::CategorisedAllocPolicy::DeallocateBytesAligned(ptr);}
/// Free the memory allocated with _CC_NEW_ARRAY_T_ALIGN.
#	define _CC_DELETE_ARRAY_T_ALIGN(ptr, T, count, align) if(ptr){for (size_t _b = 0; _b < (size_t)count; ++_b) {(ptr)[_b].~T();} ::cc::CategorisedAllocPolicy::DeallocateBytesAligned(ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define _CC_NEW_T_SIMD(T) _CC_NEW_T_ALIGN(T, CC_SIMD_ALIGNMENT)
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T_SIMD(T, count) _CC_NEW_ARRAY_T_ALIGN(T, count, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with _CC_NEW_T_SIMD.
#	define _CC_DELETE_T_SIMD(ptr, T) _CC_DELETE_T_ALIGN(ptr, T, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with _CC_NEW_ARRAY_T_SIMD.
#	define _CC_DELETE_ARRAY_T_SIMD(ptr, T, count) _CC_DELETE_ARRAY_T_ALIGN(ptr, T, CC_SIMD_ALIGNMENT)

// new / delete for classes deriving from AllocatedObject (alignment determined by per-class policy)
// Also hooks up the file/line/function params
// Can only be used with classes that derive from AllocatedObject since customized new/delete needed
#	define _CC_NEW new (__FILE__, __LINE__, __FUNCTION__)
#	define _CC_DELETE delete

#else

/// Allocate a block of raw memory.
#	define _CC_MALLOC(bytes) ::cc::CategorisedAllocPolicy::AllocateBytes(bytes)
/// Reallocate a block of raw memory.
#	define _CC_REALLOC(ptr, bytes) ::cc::CategorisedAllocPolicy::ReallocateBytes(ptr, bytes)
/// Allocate a block of memory for a primitive type.
#	define _CC_ALLOC_T(T, count) static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T)*(count)))
/// Free the memory allocated with _CC_MALLOC or _CC_ALLOC_T. Category is required to be restated to ensure the matching policy is used
#	define _CC_FREE(ptr) ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr)

/// Allocate space for one primitive type, external type or non-virtual type with constructor parameters
#	define _CC_NEW_T(T) new (::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T(T, count) ::cc::ConstructN(static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytes(sizeof(T)*(count))), count)
/// Free the memory allocated with _CC_NEW_T.
#	define _CC_DELETE_T(ptr, T) if(ptr){(ptr)->~T(); ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr);}
/// Free the memory allocated with _CC_NEW_ARRAY_T.
#	define _CC_DELETE_ARRAY_T(ptr, T, count) if(ptr){for (size_t b = 0; b < (size_t)count; ++b) {(ptr)[b].~T();} ::cc::CategorisedAllocPolicy::DeallocateBytes((void*)ptr);}

// aligned allocation
/// Allocate a block of raw memory aligned to user defined boundaries.
#	define _CC_MALLOC_ALIGN(bytes, align) ::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, bytes)
/// Allocate a block of memory for a primitive type aligned to user defined boundaries.
#	define _CC_ALLOC_T_ALIGN(T, count, align) static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T)*(count)))
/// Free the memory allocated with either _CC_MALLOC_ALIGN or _CC_ALLOC_T_ALIGN.
#	define _CC_FREE_ALIGN(ptr, align) ::cc::CategorisedAllocPolicy::DeallocateBytesAligned((void*)ptr)
/// Allocate a block of raw memory aligned to SIMD boundaries.
#	define _CC_MALLOC_SIMD(bytes) _CC_MALLOC_ALIGN(bytes, CC_SIMD_ALIGNMENT)
/// Allocate a block of memory for a primitive type aligned to SIMD boundaries.
#	define _CC_ALLOC_T_SIMD(T, count) _CC_ALLOC_T_ALIGN(T, count, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with either _CC_MALLOC_SIMD or _CC_ALLOC_T_SIMD.
#	define _CC_FREE_SIMD(ptr) _CC_FREE_ALIGN(ptr, CC_SIMD_ALIGNMENT)

/// Allocate space for one primitive type, external type or non-virtual type aligned to user defined boundaries
#	define _CC_NEW_T_ALIGN(T, align) new (::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T))) T
/// Allocate a block of memory for 'count' primitive types aligned to user defined boundaries - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T_ALIGN(T, count, align) ::cc::ConstructN(static_cast<T*>(::cc::CategorisedAllocPolicy::AllocateBytesAligned(align, sizeof(T)*(count))), count)
/// Free the memory allocated with _CC_NEW_T_ALIGN.
#	define _CC_DELETE_T_ALIGN(ptr, T, align) if(ptr){(ptr)->~T(); ::cc::CategorisedAllocPolicy::DeallocateBytesAligned((void*)ptr);}
/// Free the memory allocated with _CC_NEW_ARRAY_T_ALIGN.
#	define _CC_DELETE_ARRAY_T_ALIGN(ptr, T, count, align) if(ptr){for (size_t _b = 0; _b < (size_t)count; ++_b) { (ptr)[_b].~T();} ::cc::CategorisedAllocPolicy::DeallocateBytesAligned((void*)ptr);}
/// Allocate space for one primitive type, external type or non-virtual type aligned to SIMD boundaries
#	define _CC_NEW_T_SIMD(T) _CC_NEW_T_ALIGN(T, CC_SIMD_ALIGNMENT)
/// Allocate a block of memory for 'count' primitive types aligned to SIMD boundaries - do not use for classes that inherit from AllocatedObject
#	define _CC_NEW_ARRAY_T_SIMD(T, count) _CC_NEW_ARRAY_T_ALIGN(T, count, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with _CC_NEW_T_SIMD.
#	define _CC_DELETE_T_SIMD(ptr, T) _CC_DELETE_T_ALIGN(ptr, T, CC_SIMD_ALIGNMENT)
/// Free the memory allocated with _CC_NEW_ARRAY_T_SIMD.
#	define _CC_DELETE_ARRAY_T_SIMD(ptr, T, count) _CC_DELETE_ARRAY_T_ALIGN(ptr, T, count, CC_SIMD_ALIGNMENT)

// new / delete for classes deriving from AllocatedObject (alignment determined by per-class policy)
#	define _CC_NEW new
#	define _CC_DELETE delete

#endif

#endif // CC_MEM_DEF_H_
