#include "Camera.h"

#include "../Renderer/RenderCore.h"

#include "Skybox.h"

#define ION_BASE_CAMERA_NAME "BaseCamera"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


Camera::Camera() : 
    Node(ION_BASE_CAMERA_NAME), 
    m_type(ECameraType::ECameraType_LookAt),
    m_fov(60.0f),
    m_zNear(0.1f),
    m_zFar(256.0f),
    m_minDepth(0.0f),
    m_maxDepth(1.0f),
    m_viewPortX(0.0f),
    m_viewPortY(0.0f),
    m_scissorX(0.0f),
    m_scissorY(0.0f),
    m_viewPortWidth(1.0f),
    m_viewPortHeight(1.0f),
    m_scissorWidth(1.0f),
    m_scissorHeight(1.0f),
    m_clearDepthValue(1.0f),
    m_clearRed(1.0f),
    m_clearGreen(1.0f),
    m_clearBlue(1.0f),
    m_clearStencilValue(0),
    m_skybox(nullptr)
{
    m_nodeType = ENodeType_Camera;
}

Camera::Camera(const eosString & _name) : 
    Node(_name), 
    m_type(ECameraType::ECameraType_LookAt),
    m_fov(60.0f),
    m_zNear(0.1f),
    m_zFar(256.0f),
    m_minDepth(0.0f),
    m_maxDepth(1.0f),
    m_viewPortX(0.0f),
    m_viewPortY(0.0f),
    m_scissorX(0),
    m_scissorY(1),
    m_viewPortWidth(800.0f),
    m_viewPortHeight(600.0f),
    m_scissorWidth(800),
    m_scissorHeight(600),
    m_clearDepthValue(1.0f),
    m_clearRed(1.0f),
    m_clearGreen(1.0f),
    m_clearBlue(1.0f),
    m_clearStencilValue(0),
    m_skybox(nullptr)
{
    m_nodeType = ENodeType_Camera;
}

Camera::~Camera()
{
    RemoveSkybox();
}

Matrix Camera::PerspectiveProjectionMatrix(ionFloat _fov, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    const ionFloat field = 1.0f / tanf(0.5f * _fov);
    Matrix perspective(
        field / _aspect,    0.0f,        0.0f,                                  0.0f,
        0.0f,               -field,      0.0f,                                  0.0f,
        0.0f,               0.0f,        _zFar / (_zNear - _zFar),              -1.0f,
        0.0f,               0.0f,        (_zNear * _zFar) / (_zNear - _zFar),   0.0f
    );

    return perspective; 
}

Matrix Camera::OrthographicProjectionMatrix(ionFloat _left, ionFloat _right, ionFloat _bottom, ionFloat _top, ionFloat _zNear, ionFloat _zFar)
{
    Matrix orthographic(
        2.0f / (_right - _left),                0.0f,                                   0.0f,                       0.0f,
        0.0f,                                   2.0f / (_bottom - _top),                0.0f,                       0.0f,
        0.0f,                                   0.0f,                                   1.0f / (_zNear - _zFar),    0.0f,
        -(_right + _left) / (_right - _left),   -(_bottom + _top) / (_bottom - _top),   _zNear / (_zNear - _zFar),  1.0f
    );

    return orthographic;
}

void Camera::SetPerspectiveProjection(ionFloat _fovDeg, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    m_fov = NIX_DEG_TO_RAD(_fovDeg);
    m_zNear = _zNear;
    m_zFar = _zFar;
    m_projection = Camera::PerspectiveProjectionMatrix(m_fov, _aspect, m_zNear, m_zFar);
};

void Camera::UpdateAspectRatio(ionFloat _aspect)
{
    m_projection = Camera::PerspectiveProjectionMatrix(m_fov, _aspect, m_zNear, m_zFar);
}

void Camera::SetCameraType(ECameraType _type)
{
    m_type = _type;
}

void Camera::UpdateView()
{
    static const Matrix identity;

    const Matrix scale = identity.Scale(GetTransform().GetScale());
    const Matrix rotate = GetTransform().GetRotation().ToMatrix();
    const Matrix translate = identity.Translate(GetTransform().GetPosition());

    if (m_type == ECameraType::ECameraType_LookAt)
    {
        m_view = rotate * translate * scale;
    }
    else
    {
        m_view = scale * translate * rotate;
    }

    m_frustum.Update(m_projection, m_view);

    if (m_skybox != nullptr)
    {
        const Matrix& model = GetTransform().GetMatrixWS();

        m_skybox->UpdateUniformBuffer(m_projection, m_view, identity);
    }
}

Skybox* Camera::AddSkybox()
{
    if (m_skybox == nullptr)
    {
        m_skybox = eosNew(Skybox, ION_MEMORY_ALIGNMENT_SIZE);
    }

    return m_skybox;
}

Skybox* Camera::GetSkybox()
{
    return m_skybox;
}

void Camera::RemoveSkybox()
{
    if (m_skybox != nullptr)
    {
        eosDelete(m_skybox);
        m_skybox = nullptr;
    }
}

void Camera::RenderSkybox(RenderCore& _renderCore)
{
    if (m_skybox != nullptr)
    {
        m_skybox->Draw(_renderCore);
    }
}

void Camera::CustomRenderSkybox(RenderCore& _renderCore, VkCommandBuffer _commandBuffer, VkRenderPass _renderPass)
{
    if (m_skybox != nullptr)
    {
        m_skybox->CustomDraw(_renderCore, _commandBuffer, _renderPass);
    }
}

void Camera::SetViewportParameters(ionFloat _x /*= 0.0f*/, ionFloat _y /*= 0.0f*/, ionFloat _width /*= 1.0f*/, ionFloat _height /*= 1.0f*/, ionFloat _minDepth /*= 0.0f*/, ionFloat _maxDepth /*= 1.0f*/)
{
    ionAssertReturnVoid(_x >= 0.0f && _x <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_y >= 0.0f && _y <= 1.0f, "max depth must be between 0 and 1");
    ionAssertReturnVoid(_width >= 0.0f && _width <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_height >= 0.0f && _height <= 1.0f, "max depth must be between 0 and 1");
    ionAssertReturnVoid(_minDepth >= 0.0f && _minDepth <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_maxDepth >= 0.0f && _maxDepth <= 1.0f, "max depth must be between 0 and 1");

    m_viewPortX = _x;
    m_viewPortY = _y;
    m_viewPortWidth = _width;
    m_viewPortHeight = _height;
    m_minDepth = _minDepth;
    m_maxDepth = _maxDepth;
}

void Camera::SetScissorParameters(ionFloat _x /*= 0.0f*/, ionFloat _y /*= 0.0f*/, ionFloat _width /*= 1.0f*/, ionFloat _height /*= 1.0f*/)
{
    ionAssertReturnVoid(_x >= 0.0f && _x <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_y >= 0.0f && _y <= 1.0f, "max depth must be between 0 and 1");
    ionAssertReturnVoid(_width >= 0.0f && _width <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_height >= 0.0f && _height <= 1.0f, "max depth must be between 0 and 1");

    m_scissorX = _x;
    m_scissorY = _y;
    m_scissorWidth = _width;
    m_scissorHeight = _height;
}

void Camera::SetRenderPassParameters(ionFloat _clearDepthValue /* = 1.0f*/, ionU8 _clearStencilValue /*= 0*/, ionFloat _clearRed /*= 1.0f*/, ionFloat _clearGreen /*= 1.0f*/, ionFloat _clearBlue /*= 1.0f*/)
{
    ionAssertReturnVoid(_clearDepthValue >= 0.0f && _clearDepthValue <= 1.0f, "Clear depth must be between 0 and 1!");
    ionAssertReturnVoid(_clearRed >= 0.0f && _clearRed <= 1.0f, "Clear red must be between 0 and 1!");
    ionAssertReturnVoid(_clearGreen >= 0.0f && _clearGreen <= 1.0f, "Clear green must be between 0 and 1!");
    ionAssertReturnVoid(_clearBlue >= 0.0f && _clearBlue <= 1.0f, "Clear blue must be between 0 and 1!");

    m_clearDepthValue = _clearDepthValue;
    m_clearStencilValue = _clearStencilValue;
    m_clearRed = _clearRed;
    m_clearGreen = _clearGreen;
    m_clearBlue = _clearBlue;
}

ionFloat Lerp(ionFloat v0, ionFloat v1, ionFloat t)
{
    return (1.0f - t) * v0 + t * v1;
}

void Camera::ConputeRenderAreaViewportScissor(ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height)
{
    const ionFloat x = static_cast<ionFloat>(_x);
    const ionFloat y = static_cast<ionFloat>(_y);
    const ionFloat width = static_cast<ionFloat>(_width);
    const ionFloat height = static_cast<ionFloat>(_height);

    m_scissor.offset.x = static_cast<ionS32>(Lerp(x, width, m_scissorX));
    m_scissor.offset.y = static_cast<ionS32>(Lerp(y, height, m_scissorY));
    m_scissor.extent.width = static_cast<ionU32>(Lerp(x, width, m_scissorWidth));
    m_scissor.extent.height = static_cast<ionU32>(Lerp(y, height, m_scissorHeight));

    m_viewport.x = Lerp(x, width, m_viewPortX);
    m_viewport.y = Lerp(y, height, m_viewPortY);
    m_viewport.width = Lerp(x, width, m_viewPortWidth);
    m_viewport.height = Lerp(y, height, m_viewPortHeight);
    m_viewport.minDepth = m_minDepth;
    m_viewport.maxDepth = m_maxDepth;

    m_renderArea.offset.x = static_cast<ionS32>(m_viewport.x);
    m_renderArea.offset.y = static_cast<ionS32>(m_viewport.y);
    m_renderArea.extent.width = static_cast<ionU32>(m_viewport.width);
    m_renderArea.extent.height = static_cast<ionU32>(m_viewport.height);
}

void Camera::SetViewport(RenderCore& _renderCore)
{
    _renderCore.SetViewport(m_viewport);
}

void Camera::SetScissor(RenderCore& _renderCore)
{
    _renderCore.SetScissor(m_scissor);
}

void Camera::StartRenderPass(RenderCore& _renderCore)
{
    _renderCore.StartRenderPass(m_clearDepthValue, m_clearStencilValue, m_clearRed, m_clearGreen, m_clearBlue, m_renderArea);
}

void Camera::EndRenderPass(RenderCore& _renderCore)
{
    _renderCore.EndRenderPass();
}

void Camera::SetViewport(RenderCore& _renderCore, VkCommandBuffer _commandBuffer)
{
    _renderCore.SetViewport(_commandBuffer, m_viewport);
}

void Camera::SetScissor(RenderCore& _renderCore, VkCommandBuffer _commandBuffer)
{
    _renderCore.SetScissor(_commandBuffer, m_scissor);
}

void Camera::StartRenderPass(RenderCore& _renderCore, VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkCommandBuffer _commandBuffer, const eosVector(VkClearValue)& _clearValues)
{
    _renderCore.StartRenderPass(_renderPass, _frameBuffer, _commandBuffer, _clearValues, m_renderArea);
}

void Camera::EndRenderPass(RenderCore& _renderCore, VkCommandBuffer _commandBuffer)
{
    _renderCore.EndRenderPass(_commandBuffer);
}

ION_NAMESPACE_END