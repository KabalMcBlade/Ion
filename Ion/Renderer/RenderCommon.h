#pragma once

#include "../Core/CoreDefs.h"


ION_NAMESPACE_BEGIN

enum ERenderType : ionU32
{
    ERenderType_Linear = 1,
    ERenderType_DoubleBuffer = 2,
    ERenderType_TripleBuffer = 3
};

ION_NAMESPACE_END