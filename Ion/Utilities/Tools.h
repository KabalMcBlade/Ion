#pragma once

#include "../Core/CoreDefs.h"
#include "../Core/StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

namespace Tools
{
    ionU32 Hash32(const void* _data, ionU32 _size, ionU32 _seed = 0);
    ionU64 Hash64(const void* _data, ionU32 _size, ionU64 _seed = 0);

    // very windows Dependant
    void GetPhysicalAddress(eosString& _outAddress, ionU64& _outAddressNum);
}

ION_NAMESPACE_END