#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

//#include "../Renderer/RenderCommon.h"
#include "../Renderer/RenderState.h"


ION_NAMESPACE_BEGIN

class ION_DLL MaterialState
{
public:
    ionU64 GetStateBits() const { return m_stateBits; }
    void SetStateBits(ionU64 _stateBits) { m_stateBits = _stateBits; }

    void SetCustomBits(ionU64 _stateBits) { m_stateBits |= _stateBits; }
    void UnsetCustomBits(ionU64 _stateBits) { m_stateBits &= ~_stateBits; }

    void SetRasterizationMode(ERasterization _state) { m_stateBits |= _state; }
    void UnsetRasterizationMode(ERasterization _state) { m_stateBits &= ~_state; }

    void SetCullingMode(ECullingMode _state) { m_stateBits |= _state; }
    void UnsetCullingMode(ECullingMode _state) { m_stateBits &= ~_state; }

    void SetBlendStateMode(EBlendState _state) { m_stateBits |= _state; }
    void UnsetBlendStateMode(EBlendState _state) { m_stateBits &= ~_state; }

    void SetBlendOperatorMode(EBlendOperator _state) { m_stateBits |= _state; }
    void UnsetBlendOperatorMode(EBlendOperator _state) { m_stateBits &= ~_state; }

    void SetColorMaskMode(EColorMask _state) { m_stateBits |= _state; }
    void UnsetColorMaskMode(EColorMask _state) { m_stateBits &= ~_state; }

    void SetDepthFunctionMode(EDepthFunction _state) { m_stateBits |= _state; }
    void UnsetDepthFunctionMode(EDepthFunction _state) { m_stateBits &= ~_state; }

    void SetStencilFrontFunctionMode(EStencilFrontFunction _state) { m_stateBits |= _state; }
    void UnsetStencilFrontFunctionMode(EStencilFrontFunction _state) { m_stateBits &= ~_state; }

    void SetStencilFrontOperatorMode(EStencilFrontOperator _state) { m_stateBits |= _state; }
    void UnsetStencilFrontOperatorMode(EStencilFrontOperator _state) { m_stateBits &= ~_state; }

    void SetStencilBackFunctionMode(EStencilBackFunction _state) { m_stateBits |= _state; }
    void UnsetStencilBackFunctionMode(EStencilBackFunction _state) { m_stateBits &= ~_state; }

    void SetStencilBackOperatorMode(EStencilBackOperator _state) { m_stateBits |= _state; }
    void UnsetStencilBackOperatorMode(EStencilBackOperator _state) { m_stateBits &= ~_state; }

    void SetStencilFunctionReferenceMode(EStencilFunctionReference _state) { m_stateBits |= _state; }
    void UnsetStencilFunctionReferenceMode(EStencilFunctionReference _state) { m_stateBits &= ~_state; }

private:
    ionU64          m_stateBits;
};

ION_NAMESPACE_END