#include "Tools.h"

ION_NAMESPACE_BEGIN

namespace Tools
{
    struct InternalHash32
    {
        ionU32 m_seed;
        ionU32 m_input;
    };

    struct InternalHash64
    {
        ionU64 m_seed;
        ionU64 m_input;
    };

    template<class T> class HashGenerator;

    template<>
    class HashGenerator<InternalHash32>
    {
    public:
        ionU32 operator()(const InternalHash32& _hash) const
        {
            ionU32 a = static_cast<ionU32>(std::hash<ionU32>()(_hash.m_seed));
            ionU32 b = static_cast<ionU32>(std::hash<ionU32>()(_hash.m_input));
            return a ^ b;
        }
    };

    template<>
    class HashGenerator<InternalHash64>
    {
    public:
        ionU64 operator()(const InternalHash64& _hash) const
        {
            ionU64 a = std::hash<ionU64>()(_hash.m_seed);
            ionU64 b = std::hash<ionU64>()(_hash.m_input);
            return a ^ b;
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    ionU32 Hash32(const void* _data, ionU32 _size, ionU32 _seed /*= 0*/)
    {
        ionU32 hash = _seed;
        const char* p = reinterpret_cast<const char*>(_data);

        for (ionU32 i = 0; i < _size; ++i)
        {
            InternalHash32 ih;
            ih.m_seed = hash;
            ih.m_input = p[i];

            hash = HashGenerator<InternalHash32>()(ih);
        }
        return hash;
    }

    ionU64 Hash64(const void* _data, ionU32 _size, ionU64 _seed /*= 0*/)
    {
        ionU64 hash = _seed;
        const char* p = reinterpret_cast<const char*>(_data);

        for (ionU32 i = 0; i < _size; ++i)
        {
            InternalHash64 ih;
            ih.m_seed = hash;
            ih.m_input = p[i];

            hash = HashGenerator<InternalHash64>()(ih);
        }
        return hash;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

// Link with Iphlpapi.lib
#pragma comment(lib, "IPHLPAPI.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

    void GetPhysicalAddress(eosString& _outAddress, ionU64& _outAddressNum)
    {
        DWORD dwRetVal = 0;

        ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
        ULONG family = AF_UNSPEC;

        PIP_ADAPTER_ADDRESSES pAddresses = NULL;
        ULONG outBufLen = 0;
        ULONG Iterations = 0;

        PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
       
        do
        {
            pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
            if (pAddresses == NULL) 
            {
                printf ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
                exit(1);
            }

            dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

            if (dwRetVal == ERROR_BUFFER_OVERFLOW)
            {
                FREE(pAddresses);
                pAddresses = NULL;
            }
            else
            {
                break;
            }

            Iterations++;

        } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

        if (dwRetVal == NO_ERROR) 
        {
            bool addressFound = false;
            // If successful, output some information from the data we received
            pCurrAddresses = pAddresses;
            while (pCurrAddresses && !addressFound)
            {
                if (pCurrAddresses->PhysicalAddressLength != 0) 
                {
                    addressFound = true;

                    ionU32 c = 40;
                    for (ionU32 i = 0; i < (ionU32)pCurrAddresses->PhysicalAddressLength; i++)
                    {
                        char charBuffer[32];
                        sprintf_s(charBuffer, 32, "%.2X", (int)pCurrAddresses->PhysicalAddress[i]);
                        _outAddress.append(charBuffer);

                        _outAddressNum |= (int)pCurrAddresses->PhysicalAddress[i] << c;
                        c -= 8;
                    }
                }
              
                pCurrAddresses = pCurrAddresses->Next;
            }
        }

        if (pAddresses) 
        {
            FREE(pAddresses);
        }

    }
}

ION_NAMESPACE_END