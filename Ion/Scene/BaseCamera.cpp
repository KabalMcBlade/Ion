#include "BaseCamera.h"

#include "../Renderer/RenderCore.h"


#define ION_BASE_CAMERA_NAME "BaseCamera"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


BaseCamera::BaseCamera() : Node(ION_BASE_CAMERA_NAME)
{
    m_type = ECameraType::ECameraType_LookAt;

    m_nodeType = ENodeType_Camera;
}

BaseCamera::BaseCamera(const eosString & _name) : Node(_name)
{
    m_type = ECameraType::ECameraType_LookAt;

    m_nodeType = ENodeType_Camera;
}

BaseCamera::~BaseCamera()
{

}


Matrix BaseCamera::PerspectiveProjectionMatrix(ionFloat _fov, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    // This is the normalized device space of OpenGL:
    // Left hand
    // -1 to 1 depth
    /*
    ionFloat field = 1.0f / tanf(0.5f * _fov);
    Matrix perspective(
        field / _aspect,    0.0f,       0.0f,                                       0.0f,
        0.0f,               field,      0.0f,                                       0.0f,
        0.0f,               0.0f,       (_zFar + _zNear) / (_zNear - _zFar),        -1.0f,
        0.0f,               0.0f,       (2.0f * _zFar * _zNear) / (_zNear - _zFar), 0.0f
    );
    */

    /*
    // This is the normalized device space for Vulkan
    // Right Hand
    // 0 to 1 depth
    ionFloat field = tanf(0.5f * _fov);
    Matrix perspective(
        1 / (_aspect * field),  0.0f,           0.0f,                                       0.0f,
        0.0f,                   1 / field,      0.0f,                                       0.0f,
        0.0f,                   0.0f,           _zFar / (_zNear - _zFar),                   -1.0f,
        0.0f,                   0.0f,           -(_zFar * _zNear) / (_zFar - _zNear),       0.0f
    );
    */

    // With this perspective projection, I don't need anymoire to swap Y in the GLTF.
    // now Y are UP and -Y are DOWN
    ionFloat field = 1.0f / tanf(0.5f * _fov);
    Matrix perspective(
        field / _aspect,    0.0f,        0.0f,                                       0.0f,
        0.0f,               -field,      0.0f,                                       0.0f,
        0.0f,               0.0f,        _zFar / (_zNear - _zFar),                   -1.0f,
        0.0f,               0.0f,        (_zNear * _zFar) / (_zNear - _zFar),       0.0f
    );

    return perspective;
}

Matrix BaseCamera::OrthographicProjectionMatrix(ionFloat _left, ionFloat _right, ionFloat _bottom, ionFloat _top, ionFloat _zNear, ionFloat _zFar)
{
    Matrix orthographic(
        2.0f / (_right - _left),                0.0f,                                   0.0f,                       0.0f,
        0.0f,                                   2.0f / (_bottom - _top),                0.0f,                       0.0f,
        0.0f,                                   0.0f,                                   1.0f / (_zNear - _zFar),    0.0f,
        -(_right + _left) / (_right - _left),   -(_bottom + _top) / (_bottom - _top),   _zNear / (_zNear - _zFar),  1.0f
    );

    return orthographic;
}

void BaseCamera::SetPerspectiveProjection(ionFloat _fovDeg, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    m_fov = NIX_DEG_TO_RAD(_fovDeg);
    m_zNear = _zNear;
    m_zFar = _zFar;
    m_projection = BaseCamera::PerspectiveProjectionMatrix(m_fov, _aspect, m_zNear, m_zFar);
};

void BaseCamera::UpdateAspectRatio(ionFloat _aspect)
{
    m_projection = BaseCamera::PerspectiveProjectionMatrix(m_fov, _aspect, m_zNear, m_zFar);
}

void BaseCamera::SetCameraType(ECameraType _type)
{
    m_type = _type;
}

void BaseCamera::UpdateViewMatrix()
{
    Matrix parent;
    if (GetParentHandle().IsValid())
    {
        parent = GetParentHandle()->GetTransformHandle()->GetMatrix();
    }

    if (m_type == ECameraType::ECameraType_FirstPerson)
    {
        GetTransformHandle()->UpdateTransformInverse(parent);
        m_view = GetTransformHandle()->GetMatrixInverse();
    }
    else
    {
        GetTransformHandle()->UpdateTransform(parent);
        m_view = GetTransformHandle()->GetMatrix();
    }

    m_frustum.Update(m_projection, m_view);
}

void BaseCamera::UpdateMovement()
{

}

void BaseCamera::Update()
{
    UpdateMovement();
    UpdateViewMatrix();
}

void BaseCamera::SetViewport(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight, ionFloat _minDepth, ionFloat _maxDepth)
{
    ionAssertReturnVoid(_percentageOfWithHeight >= 0.0f && _percentageOfWithHeight <= 1.0f, "pergentage must be between 0 and 1");
    ionAssertReturnVoid(_minDepth >= 0.0f && _minDepth <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_maxDepth >= 0.0f && _maxDepth <= 1.0f, "max depth must be between 0 and 1");

    _renderCore.SetViewport(static_cast<ionFloat>(_fromX), static_cast<ionFloat>(_fromY), static_cast<ionFloat>(_width)  * _percentageOfWithHeight, static_cast<ionFloat>(_height)  * _percentageOfWithHeight, _minDepth, _maxDepth);
}

void BaseCamera::SetScissor(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight)
{
    ionAssertReturnVoid(_percentageOfWithHeight >= 0.0f && _percentageOfWithHeight <= 1.0f, "pergentage must be between 0 and 1");

    _renderCore.SetScissor(_fromX, _fromY, static_cast<ionU32>((static_cast<ionFloat>(_width)  * _percentageOfWithHeight)), static_cast<ionU32>((static_cast<ionFloat>(_height)  * _percentageOfWithHeight)));
}

void BaseCamera::StartRenderPass(RenderCore& _renderCore, ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue)
{
    ionAssertReturnVoid(_clearDepthValue >= 0.0f && _clearDepthValue <= 1.0f, "Clear depth must be between 0 and 1!");
    ionAssertReturnVoid(_clearRed >= 0.0f && _clearRed <= 1.0f, "Clear red must be between 0 and 1!");
    ionAssertReturnVoid(_clearGreen >= 0.0f && _clearGreen <= 1.0f, "Clear green must be between 0 and 1!");
    ionAssertReturnVoid(_clearBlue >= 0.0f && _clearBlue <= 1.0f, "Clear blue must be between 0 and 1!");

    _renderCore.StartRenderPass(_clearDepthValue, _clearStencilValue, _clearRed, _clearGreen, _clearBlue);
}

void BaseCamera::EndRenderPass(RenderCore& _renderCore)
{
    _renderCore.EndRenderPass();
}


ION_NAMESPACE_END