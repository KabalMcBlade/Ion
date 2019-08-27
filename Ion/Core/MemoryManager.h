#pragma once

#include "CoreDefs.h"
#include "../Dependencies/Eos/Eos/Eos.h"


#define ION_MEMORY_1_MB             (1u << 20u)
#define ION_MEMORY_2_MB             (1u << 21u)
#define ION_MEMORY_4_MB             (1u << 22u)
#define ION_MEMORY_8_MB             (1u << 23u)
#define ION_MEMORY_16_MB            (1u << 24u)
#define ION_MEMORY_32_MB            (1u << 25u)			// from this point on, the real size is bigger, but is a good approximation the define name given
#define ION_MEMORY_64_MB            (1u << 26u)
#define ION_MEMORY_128_MB           (1u << 27u)
#define ION_MEMORY_256_MB			(1u << 28u)
#define ION_MEMORY_512_MB           (1u << 29u)
#define ION_MEMORY_1024_MB          (1u << 30u)
#define ION_MEMORY_2048_MB          (1u << 31u)
#define ION_MEMORY_1_GB				ION_MEMORY_1024_MB
#define ION_MEMORY_2_GB				ION_MEMORY_2048_MB

// From here on, only 64 bit
#define ION_MEMORY_4096_MB          (1ULL << 32ULL)
#define ION_MEMORY_8192_MB          (1ULL << 33ULL)

   
#define ION_MAX_MEMORY_SIZE_GENERIC     ION_MEMORY_512_MB
#define ION_MAX_MEMORY_SIZE_VECTOR      ION_MEMORY_128_MB
#define ION_MAX_MEMORY_SIZE_MAP         ION_MEMORY_64_MB
#define ION_MAX_MEMORY_SIZE_STRING      ION_MEMORY_4_MB
#define ION_MAX_MEMORY_SIZE_SMARTPTR    ION_MEMORY_32_MB
//#define ION_MAX_MEMORY_SIZE_STREAM		ION_MEMORY_4_MB


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


// the second number in the template is the "level" for the pow2 in memory (the log2 result from the total memory desired)
using HeapAllocationPolicyGeneric = eosAllocationPolicy<eosHeapAllocator<16, 29>, eosAllocationHeaderU32>;
using HeapAllocationPolicyVector = eosAllocationPolicy<eosHeapAllocator<16, 27>, eosAllocationHeaderU32>;
using HeapAllocationPolicyMap = eosAllocationPolicy<eosHeapAllocator<16, 26>, eosAllocationHeaderU32>;
using HeapAllocationPolicyString = eosAllocationPolicy<eosHeapAllocator<16, 22>, eosAllocationHeaderU32>;
using HeapAllocationPolicySmartPointer = eosAllocationPolicy<eosHeapAllocator<16, 25>, eosAllocationHeaderU32>;
// using HeapAllocationPolicyGeneric = eosAllocationPolicy<eosMallocAllocator<16>, eosAllocationHeaderU32>;
// using HeapAllocationPolicyVector = eosAllocationPolicy<eosMallocAllocator<16>, eosAllocationHeaderU32>;
// using HeapAllocationPolicyMap = eosAllocationPolicy<eosMallocAllocator<16>, eosAllocationHeaderU32>;
// using HeapAllocationPolicyString = eosAllocationPolicy<eosMallocAllocator<16>, eosAllocationHeaderU32>;
// using HeapAllocationPolicySmartPointer = eosAllocationPolicy<eosMallocAllocator<16>, eosAllocationHeaderU32>;

using HeapAllocatorGeneric = eosAllocator<HeapAllocationPolicyGeneric, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
using HeapAllocatorVector = eosAllocator<HeapAllocationPolicyVector, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
using HeapAllocatorMap = eosAllocator<HeapAllocationPolicyMap, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
using HeapAllocatorString = eosAllocator<HeapAllocationPolicyString, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;
using HeapAllocatorSmartPointer = eosAllocator<HeapAllocationPolicySmartPointer, eosDefaultSingleThreadPolicy, eosDefaultBoundsCheckingPolicy, eosDefaultTrackingPolicy, eosDefaultTaggingPolicy>;

class ION_DLL MemoryManager
{
public:
    static MemoryManager& Instance()
    {
        static MemoryManager instance;
        return instance;
    }

	HeapAllocatorGeneric* GetGenericAllocator() { return &m_genericHeapAllocator; }
	HeapAllocatorVector* GetVectorAllocator() { return &m_vectorHeapAllocator; }
	HeapAllocatorMap* GetMapAllocator() { return &m_mapHeapAllocator; }
	HeapAllocatorString* GetStringAllocator() { return &m_stringHeapAllocator; }
	HeapAllocatorSmartPointer* GetSmartPointerAllocator() { return &m_smartPointerHeapAllocator; }
    //HeapAllocator* GetStreamPointerAllocator() { return &m_streamPointerHeapAllocator; }

private:
    MemoryManager();

private:
	eosHeapArea m_genericHeapAllocatorArea;
	eosHeapArea m_vectorHeapAllocatorArea;
	eosHeapArea m_mapHeapAllocatorArea;
	eosHeapArea m_stringHeapAllocatorArea;
	eosHeapArea m_smartPointerHeapAllocatorArea;

// 	eosDynamicHeapArea m_genericHeapAllocatorArea;
// 	eosDynamicHeapArea m_vectorHeapAllocatorArea;
// 	eosDynamicHeapArea m_mapHeapAllocatorArea;
// 	eosDynamicHeapArea m_stringHeapAllocatorArea;
// 	eosDynamicHeapArea m_smartPointerHeapAllocatorArea;
	

	HeapAllocatorGeneric m_genericHeapAllocator;
	HeapAllocatorVector m_vectorHeapAllocator;
	HeapAllocatorMap m_mapHeapAllocator;
	HeapAllocatorString m_stringHeapAllocator;
	HeapAllocatorSmartPointer m_smartPointerHeapAllocator;
    //HeapAllocator m_streamPointerHeapAllocator;
};


ION_NAMESPACE_END