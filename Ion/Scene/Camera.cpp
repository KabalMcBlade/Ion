#include "Camera.h"

#include "../Renderer/RenderCore.h"


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
    m_clearStencilValue(0)
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
    m_clearStencilValue(0)
{
    m_nodeType = ENodeType_Camera;
}

Camera::~Camera()
{

}

Matrix Camera::PerspectiveProjectionMatrix(ionFloat _fov, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    ionFloat field = 1.0f / tanf(0.5f * _fov);
    Matrix perspective(
        field / _aspect,    0.0f,        0.0f,                                       0.0f,
        0.0f,               -field,      0.0f,                                       0.0f,
        0.0f,               0.0f,        _zFar / (_zNear - _zFar),                   -1.0f,
        0.0f,               0.0f,        (_zNear * _zFar) / (_zNear - _zFar),       0.0f
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
    if (m_type == ECameraType::ECameraType_LookAt)
    {
        m_view = GetTransformHandle()->GetMatrixInverseWS();
    }
    else
    {
        m_view = GetTransformHandle()->GetMatrixWS();
    }

    m_frustum.Update(m_projection, m_view);
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

void Camera::SetViewport(RenderCore& _renderCore, ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height)
{
    const ionFloat x = static_cast<ionFloat>(_x);
    const ionFloat y = static_cast<ionFloat>(_y);
    const ionFloat width = static_cast<ionFloat>(_width);
    const ionFloat height = static_cast<ionFloat>(_height);

    const ionFloat newX = Lerp(x, width, m_viewPortX);
    const ionFloat newWidth = Lerp(x, width, m_viewPortWidth);

    const ionFloat newY = Lerp(y, height, m_viewPortY);
    const ionFloat newHeight = Lerp(y, height, m_viewPortHeight);

    _renderCore.SetViewport(newX, newY, newWidth, newHeight, m_minDepth, m_maxDepth);
}

void Camera::SetScissor(RenderCore& _renderCore, ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height)
{
    const ionFloat x = static_cast<ionFloat>(_x);
    const ionFloat y = static_cast<ionFloat>(_y);
    const ionFloat width = static_cast<ionFloat>(_width);
    const ionFloat height = static_cast<ionFloat>(_height);

    const ionFloat newX = Lerp(x, width, m_scissorX);
    const ionFloat newWidth = Lerp(x, width, m_scissorWidth);

    const ionFloat newY = Lerp(y, height, m_scissorY);
    const ionFloat newHeight = Lerp(y, height, m_scissorHeight);

    _renderCore.SetScissor(static_cast<ionS32>(newX), static_cast<ionS32>(newY), static_cast<ionS32>(newWidth), static_cast<ionS32>(newHeight));
}

void Camera::StartRenderPass(RenderCore& _renderCore)
{
    _renderCore.StartRenderPass(m_clearDepthValue, m_clearStencilValue, m_clearRed, m_clearGreen, m_clearBlue);
}

void Camera::EndRenderPass(RenderCore& _renderCore)
{
    _renderCore.EndRenderPass();
}


ION_NAMESPACE_END