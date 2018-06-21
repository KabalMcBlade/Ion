#pragma once

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN


#define ION_STENCIL_SHADOW_TEST_VALUE        128
#define ION_STENCIL_SHADOW_MASK_VALUE        255


#define ION_STENCIL_FUNC_REF_SHIFT  20ULL
#define ION_STENCIL_FUNC_REF_BITS   0xFFll << ION_STENCIL_FUNC_REF_SHIFT

#define ION_STENCIL_FUNC_MASK_SHIFT 28ULL
#define ION_STENCIL_FUNC_MASK_BITS  0xFFll << ION_STENCIL_FUNC_MASK_SHIFT

#define ION_STENCIL_MAKE_REF( x ) ( ( (ionU64)(x) << ION_STENCIL_FUNC_REF_SHIFT ) & ION_STENCIL_FUNC_REF_BITS )
#define ION_STENCIL_MAKE_MASK( x ) ( ( (ionU64)(x) << ION_STENCIL_FUNC_MASK_SHIFT ) & ION_STENCIL_FUNC_MASK_BITS )

enum EStencilFrontFunction : ionU64
{
    EStencilFrontFunction_Always = 0ULL << 36,
    EStencilFrontFunction_Lesser = 1ULL << 36,
    EStencilFrontFunction_LesserOrEqual = 2ULL << 36,
    EStencilFrontFunction_Greater = 3ULL << 36,
    EStencilFrontFunction_GreaterOrEqual = 4ULL << 36,
    EStencilFrontFunction_Equal = 5ULL << 36,
    EStencilFrontFunction_NotEqual = 6ULL << 36,
    EStencilFrontFunction_Never = 7ULL << 36,
    EStencilFrontFunction_Bits = 7ULL << 36
};

enum EStencilBackFunction : ionU64
{
    EStencilBackFunction_Always = 0ULL << 48,
    EStencilBackFunction_Lesser = 1ULL << 48,
    EStencilBackFunction_LesserOrEqual = 2ULL << 48,
    EStencilBackFunction_Greater = 3ULL << 48,
    EStencilBackFunction_GreaterOrEqual = 4ULL << 48,
    EStencilBackFunction_Equal = 5ULL << 48,
    EStencilBackFunction_NotEqual = 6ULL << 48,
    EStencilBackFunction_Never = 7ULL << 48,
    EStencilBackFunction_Bits = 7ULL << 48
};

enum EStencilBackOperator : ionU64
{
    EStencilBackOperator_Fail_Keep = 0ULL << 51,
    EStencilBackOperator_Fail_Zero = 1ULL << 51,
    EStencilBackOperator_Fail_Replace = 2ULL << 51,
    EStencilBackOperator_Fail_Increment = 3ULL << 51,
    EStencilBackOperator_Fail_Decrement = 4ULL << 51,
    EStencilBackOperator_Fail_Invert = 5ULL << 51,
    EStencilBackOperator_Fail_Increment_Wrap = 6ULL << 51,
    EStencilBackOperator_Fail_Decrement_Wrap = 7ULL << 51,
    EStencilBackOperator_Fail_Bits = 7ULL << 51,

    EStencilBackOperator_ZFail_Keep = 0ULL << 54,
    EStencilBackOperator_ZFail_Zero = 1ULL << 54,
    EStencilBackOperator_ZFail_Replace = 2ULL << 54,
    EStencilBackOperator_ZFail_Increment = 3ULL << 54,
    EStencilBackOperator_ZFail_Decrement = 4ULL << 54,
    EStencilBackOperator_ZFail_Invert = 5ULL << 54,
    EStencilBackOperator_ZFail_Increment_Wrap = 6ULL << 54,
    EStencilBackOperator_ZFail_Decrement_Wrap = 7ULL << 54,
    EStencilBackOperator_ZFail_Bits = 7ULL << 54,

    EStencilBackOperator_Pass_Keep = 0ULL << 57,
    EStencilBackOperator_Pass_Zero = 1ULL << 57,
    EStencilBackOperator_Pass_Replace = 2ULL << 57,
    EStencilBackOperator_Pass_Increment = 3ULL << 57,
    EStencilBackOperator_Pass_Decrement = 4ULL << 57,
    EStencilBackOperator_Pass_Invert = 5ULL << 57,
    EStencilBackOperator_Pass_Increment_Wrap = 6ULL << 57,
    EStencilBackOperator_Pass_Decrement_Wrap = 7ULL << 57,
    EStencilBackOperator_Pass_Bits = 7ULL << 57,

    EStencilBackOperator_Bits = EStencilBackOperator_Fail_Bits | EStencilBackOperator_ZFail_Bits | EStencilBackOperator_Pass_Bits,
    EStencilSeparate_Stencil = EStencilBackOperator_Bits
};

enum EStencilFrontOperator : ionU64
{
    EStencilFrontOperator_Fail_Keep = 0ULL << 39,
    EStencilFrontOperator_Fail_Zero = 1ULL << 39,
    EStencilFrontOperator_Fail_Replace = 2ULL << 39,
    EStencilFrontOperator_Fail_Increment = 3ULL << 39,
    EStencilFrontOperator_Fail_Decrement = 4ULL << 39,
    EStencilFrontOperator_Fail_Invert = 5ULL << 39,
    EStencilFrontOperator_Fail_Increment_Wrap = 6ULL << 39,
    EStencilFrontOperator_Fail_Decrement_Wrap = 7ULL << 39,
    EStencilFrontOperator_Fail_Bits = 7ULL << 39,

    EStencilFrontOperator_ZFail_Keep = 0ULL << 42,
    EStencilFrontOperator_ZFail_Zero = 1ULL << 42,
    EStencilFrontOperator_ZFail_Replace = 2ULL << 42,
    EStencilFrontOperator_ZFail_Increment = 3ULL << 42,
    EStencilFrontOperator_ZFail_Decrement = 4ULL << 42,
    EStencilFrontOperator_ZFail_Invert = 5ULL << 42,
    EStencilFrontOperator_ZFail_Increment_Wrap = 6ULL << 42,
    EStencilFrontOperator_ZFail_Decrement_Wrap = 7ULL << 42,
    EStencilFrontOperator_ZFail_Bits = 7ULL << 42,

    EStencilFrontOperator_Pass_Keep = 0ULL << 45,
    EStencilFrontOperator_Pass_Zero = 1ULL << 45,
    EStencilFrontOperator_Pass_Replace = 2ULL << 45,
    EStencilFrontOperator_Pass_Increment = 3ULL << 45,
    EStencilFrontOperator_Pass_Decrement = 4ULL << 45,
    EStencilFrontOperator_Pass_Invert = 5ULL << 45,
    EStencilFrontOperator_Pass_Increment_Wrap = 6ULL << 45,
    EStencilFrontOperator_Pass_Decrement_Wrap = 7ULL << 45,
    EStencilFrontOperator_Pass_Bits = 7ULL << 45,

    EStencilFrontOperator_Bits = EStencilFrontOperator_Fail_Bits | EStencilFrontOperator_ZFail_Bits | EStencilFrontOperator_Pass_Bits,
    EStencilOperator_Bits = EStencilFrontOperator_Bits | EStencilSeparate_Stencil
};

enum EStencilFunctionReference : ionU64
{
    EStencilFunctionReference_RefShift = 20,
    EStencilFunctionReference_RefBits = 0xFFLL << EStencilFunctionReference_RefShift,

    EStencilFunctionReference_MaskShift = 28,
    EStencilFunctionReference_MaskBits = 0xFFLL << EStencilFunctionReference_MaskShift

};

enum ERasterization : ionU64
{
    ERasterization_PolygonMode_Line = 1 << 11,
    ERasterization_PolygonMode_Offset = 1 << 12,

    ERasterization_DepthTest_Mask = 1ULL << 60,
    ERasterization_Face_Clockwise = 1ULL << 61,
    ERasterization_View_Specular = 1ULL << 62
};


enum ECullingMode : ionU64
{
    ECullingMode_Front = 0 << 15,
    ECullingMode_Back = 1 << 15,
    ECullingMode_TwoSide = 2 << 15,
    ECullingMode_Bits = 7 << 15,
    ECullingMode_Mask = ECullingMode_Front | ECullingMode_Back | ECullingMode_TwoSide,
};

enum EBlendState : ionU64
{
    EBlendState_Source_One = 0 << 0,
    EBlendState_Source_Zero = 1 << 0,
    EBlendState_Source_Dest_Color = 2 << 0,
    EBlendState_Source_One_Minus_Dest_Color = 3 << 0,
    EBlendState_Source_Source_Alpha = 4 << 0,
    EBlendState_Source_One_Minus_Source_Alpha = 5 << 0,
    EBlendState_Source_Dest_Alpha = 6 << 0,
    EBlendState_Source_One_Minus_Dest_Alpha = 7 << 0,
    EBlendState_Source_Bits = 7 << 0,

    EBlendState_Dest_Zero = 0 << 3,
    EBlendState_Dest_One = 1 << 3,
    EBlendState_Dest_Source_Color = 2 << 3,
    EBlendState_Dest_One_Minus_Source_Color = 3 << 3,
    EBlendState_Dest_Source_Alpha = 4 << 3,
    EBlendState_Dest_One_Minus_Source_Alpha = 5 << 3,
    EBlendState_Dest_Dest_Alpha = 6 << 3,
    EBlendState_Dest_One_Minus_Dest_Alpha = 7 << 3,
    EBlendState_Dest_Bits = 7 << 3
};

enum EBlendOperator : ionU64
{
    EBlendOperator_Add = 0 << 18,
    EBlendOperator_Sub = 1 << 18,
    EBlendOperator_Min = 2 << 18,
    EBlendOperator_Max = 3 << 18,
    EBlendOperator_Bits = 3 << 18
};

enum EColorMask : ionU64
{
    EColorMask_Depth = 1 << 6,
    EColorMask_Red = 1 << 7,
    EColorMask_Green = 1 << 8,
    EColorMask_Blue = 1 << 9,
    EColorMask_Alpha = 1 << 10,
    EColorMask_Color = (EColorMask_Red | EColorMask_Green | EColorMask_Blue)
};

enum EDepthFunction : ionU64
{
    EDepthFunction_Less = 0 << 13,
    EDepthFunction_Always = 1 << 13,
    EDepthFunction_Greater = 2 << 13,
    EDepthFunction_Equal = 3 << 13,
    EDepthFunction_Bits = 3 << 13
};



ION_NAMESPACE_END