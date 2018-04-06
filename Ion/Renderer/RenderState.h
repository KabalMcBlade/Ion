#pragma once

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN


enum EStencilFrontFunction : ionU64
{
    EStencilFrontFunction_Always = 0ull << 36,
    EStencilFrontFunction_Lesser = 1ull << 36,
    EStencilFrontFunction_LesserOrEqual = 2ull << 36,
    EStencilFrontFunction_Greater = 3ull << 36,
    EStencilFrontFunction_GreaterOrEqual = 4ull << 36,
    EStencilFrontFunction_Equal = 5ull << 36,
    EStencilFrontFunction_NotEqual = 6ull << 36,
    EStencilFrontFunction_Never = 7ull << 36,
    EStencilFrontFunction_Bits = 7ull << 36
};

enum EStencilBackFunction : ionU64
{
    EStencilBackFunction_Always = 0ull << 48,
    EStencilBackFunction_Lesser = 1ull << 48,
    EStencilBackFunction_LesserOrEqual = 2ull << 48,
    EStencilBackFunction_Greater = 3ull << 48,
    EStencilBackFunction_GreaterOrEqual = 4ull << 48,
    EStencilBackFunction_Equal = 5ull << 48,
    EStencilBackFunction_NotEqual = 6ull << 48,
    EStencilBackFunction_Never = 7ull << 48,
    EStencilBackFunction_Bits = 7ull << 48
};

enum EStencilFrontOperator : ionU64
{
    EStencilFrontOperator_Fail_Keep = 0ull << 39,
    EStencilFrontOperator_Fail_Zero = 1ull << 39,
    EStencilFrontOperator_Fail_Replace = 2ull << 39,
    EStencilFrontOperator_Fail_Increment = 3ull << 39,
    EStencilFrontOperator_Fail_Decrement = 4ull << 39,
    EStencilFrontOperator_Fail_Invert = 5ull << 39,
    EStencilFrontOperator_Fail_Increment_Wrap = 6ull << 39,
    EStencilFrontOperator_Fail_Decrement_Wrap = 7ull << 39,
    EStencilFrontOperator_Fail_Bits = 7ull << 39,

    EStencilFrontOperator_ZFail_Keep = 0ull << 42,
    EStencilFrontOperator_ZFail_Zero = 1ull << 42,
    EStencilFrontOperator_ZFail_Replace = 2ull << 42,
    EStencilFrontOperator_ZFail_Increment = 3ull << 42,
    EStencilFrontOperator_ZFail_Decrement = 4ull << 42,
    EStencilFrontOperator_ZFail_Invert = 5ull << 42,
    EStencilFrontOperator_ZFail_Increment_Wrap = 6ull << 42,
    EStencilFrontOperator_ZFail_Decrement_Wrap = 7ull << 42,
    EStencilFrontOperator_ZFail_Bits = 7ull << 42,

    EStencilFrontOperator_Pass_Keep = 0ull << 45,
    EStencilFrontOperator_Pass_Zero = 1ull << 45,
    EStencilFrontOperator_Pass_Replace = 2ull << 45,
    EStencilFrontOperator_Pass_Increment = 3ull << 45,
    EStencilFrontOperator_Pass_Decrement = 4ull << 45,
    EStencilFrontOperator_Pass_Invert = 5ull << 45,
    EStencilFrontOperator_Pass_Increment_Wrap = 6ull << 45,
    EStencilFrontOperator_Pass_Decrement_Wrap = 7ull << 45,
    EStencilFrontOperator_Pass_Bits = 7ull << 45
};

enum EStencilBackOperator : ionU64
{
    EStencilBackOperator_Fail_Keep = 0ull << 51,
    EStencilBackOperator_Fail_Zero = 1ull << 51,
    EStencilBackOperator_Fail_Replace = 2ull << 51,
    EStencilBackOperator_Fail_Increment = 3ull << 51,
    EStencilBackOperator_Fail_Decrement = 4ull << 51,
    EStencilBackOperator_Fail_Invert = 5ull << 51,
    EStencilBackOperator_Fail_Increment_Wrap = 6ull << 51,
    EStencilBackOperator_Fail_Decrement_Wrap = 7ull << 51,
    EStencilBackOperator_Fail_Bits = 7ull << 51,

    EStencilBackOperator_ZFail_Keep = 0ull << 54,
    EStencilBackOperator_ZFail_Zero = 1ull << 54,
    EStencilBackOperator_ZFail_Replace = 2ull << 54,
    EStencilBackOperator_ZFail_Increment = 3ull << 54,
    EStencilBackOperator_ZFail_Decrement = 4ull << 54,
    EStencilBackOperator_ZFail_Invert = 5ull << 54,
    EStencilBackOperator_ZFail_Increment_Wrap = 6ull << 54,
    EStencilBackOperator_ZFail_Decrement_Wrap = 7ull << 54,
    EStencilBackOperator_ZFail_Bits = 7ull << 54,

    EStencilBackOperator_Pass_Keep = 0ull << 57,
    EStencilBackOperator_Pass_Zero = 1ull << 57,
    EStencilBackOperator_Pass_Replace = 2ull << 57,
    EStencilBackOperator_Pass_Increment = 3ull << 57,
    EStencilBackOperator_Pass_Decrement = 4ull << 57,
    EStencilBackOperator_Pass_Invert = 5ull << 57,
    EStencilBackOperator_Pass_Increment_Wrap = 6ull << 57,
    EStencilBackOperator_Pass_Decrement_Wrap = 7ull << 57,
    EStencilBackOperator_Pass_Bits = 7ull << 57
};



ION_NAMESPACE_END