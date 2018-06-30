#pragma once

#include "StandardIncludes.h"


#ifdef ION_EXPORTS
#define ION_DLL __declspec(dllexport)
#else
#define ION_DLL __declspec(dllimport)
#endif 


// defines for easy namespace-ing
#define ION_NAMESPACE_BEGIN namespace ion {
#define ION_NAMESPACE_END };  

#define ION_USING_NAMESPACE using namespace ion; 

#define ION_MEMORY_ALIGNMENT_SIZE 16


#define ION_OPTIMIZATION_OFF __pragma(optimize("",off))
#define ION_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define ION_INLINE    __forceinline

// tells the compiler to never inline a particular function
#define ION_NO_INLINE    __declspec(noinline)

// Memory alignment
#define ION_MEMORY_ALIGNMENT(x)    __declspec(align(x))
#define ION_IS_ALIGNED(ptr, alignment)    ((uint_fast64_t)ptr & (alignment - 1)) == 0

// BIT MANIPULATION
#define ION_BIT_SET(value, bitpos)          ((value) |= (1<<(bitpos)))
#define ION_BIT_SET_IFF(value, iff, bitpos) ((value) ^= (-iff ^ (value)) & (1 << (bitpos)))
#define ION_BIT_SET_VALUE(value, mask, set) ((value) = (((value) & (mask)) | (set)))

#define ION_BIT_CHECK(value, bitpos)        ((value) & (1<<(bitpos))) 
#define ION_BIT_CLEAR(value, bitpos)        ((value) &= ~((1) << (bitpos)))
#define ION_BIT_TOGGLE(value, bitpos)       ((value) ^= (1<<(bitpos)))
#define ION_BIT_GET(value, mask)            ((value) & (mask)) 

#define ION_UNUSED(x)   (void)(x)

#define ION_SCOPE_BEGIN \
{

#define ION_SCOPE_END \
}

//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

typedef bool            ionBool;
typedef float           ionFloat;
typedef double          ionDouble;

typedef uint8_t         ionU8;
typedef uint16_t        ionU16;
typedef uint32_t        ionU32;
typedef uint64_t        ionU64;
typedef int8_t          ionS8;
typedef int16_t         ionS16;
typedef int32_t         ionS32;
typedef int64_t         ionS64;
typedef std::size_t     ionSize;


//////////////////////////////////////////////////////////////////////////
// ASSERT
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define ionAssert( condition, message ) \
    if( !(condition) ) { \
        std::cerr << "Assert: " << (#condition) << std::endl; \
        std::cerr << "Message: " << message << std::endl; \
        std::cerr << "File: " << __FILE__ << std::endl; \
        std::cerr << "Line: " << __LINE__ << std::endl << std::endl; \
    }
#define ionAssertDialog( condition ) assert(condition)
#else
#define ionAssert( condition, message )
#define ionAssertDialog( condition )
#endif // DEBUG

#define ionAssertReturnVoid( condition, message ) \
    ionAssert( condition, message )\
    if( !(condition) ) { \
        return;\
    }

#define ionAssertReturnValue( condition, message, return_value ) \
    ionAssert( condition, message )\
    if( !(condition) ) { \
        return return_value;\
    }