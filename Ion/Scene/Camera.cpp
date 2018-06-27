#include "Camera.h"

#include "../Renderer/RenderCore.h"


#define ION_BASE_CAMERA_NAME "BaseCamera"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


Camera::Camera() : Node(ION_BASE_CAMERA_NAME), m_movementSpeed(1.0f), m_mouseSensitivity(0.25f), m_pitchDeg(0.0f), m_yawDeg(0.0f)
{
    m_type = ECameraType::ECameraType_LookAt;

    m_nodeType = ENodeType_Camera;
}

Camera::Camera(const eosString & _name) : Node(_name), m_movementSpeed(1.0f), m_mouseSensitivity(0.25f), m_pitchDeg(0.0f), m_yawDeg(0.0f)
{
    m_type = ECameraType::ECameraType_LookAt;

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
    if (m_type == ECameraType::ECameraType_FirstPerson)
    {
        m_view = GetTransformHandle()->GetMatrixInverseWS();
    }
    else
    {
        m_view = GetTransformHandle()->GetMatrixWS();
    }

    m_frustum.Update(m_projection, m_view);
}

void Camera::ProcessMouseMovement(ionFloat _xOffset, ionFloat _yOffset, ionBool _constrainPitch /*= true*/)
{
    _xOffset *=  m_mouseSensitivity;
    _yOffset *=  m_mouseSensitivity;

    m_yawDeg += 0.5f * _xOffset;
    m_pitchDeg -= 0.5f * _yOffset;

    if (_constrainPitch) 
    {
        if (m_pitchDeg > 89.0f) 
        {
            m_pitchDeg = 89.0f;
        }
        if (m_pitchDeg < -89.0f)
        {
            m_pitchDeg = -89.0f;
        }
    }

    static const Vector up(0.0f, 1.0f, 0.0f, 1.0f);
    static const Vector right(1.0f, 0.0f, 0.0f, 1.0f);
    static const Vector forward(0.0f, 0.0f, 1.0f, 1.0f);

    const Quaternion cameraRotYaw(NIX_DEG_TO_RAD(m_yawDeg), up);
    const Quaternion cameraRotPitch(NIX_DEG_TO_RAD(m_pitchDeg), right);
    const Quaternion cameraRotRoll(NIX_DEG_TO_RAD(0.0f), forward);

    const Quaternion rotation(cameraRotRoll * cameraRotPitch * cameraRotYaw);
    
    GetTransformHandle()->SetRotation(rotation);
}

void Camera::SetViewport(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight, ionFloat _minDepth, ionFloat _maxDepth)
{
    ionAssertReturnVoid(_percentageOfWithHeight >= 0.0f && _percentageOfWithHeight <= 1.0f, "pergentage must be between 0 and 1");
    ionAssertReturnVoid(_minDepth >= 0.0f && _minDepth <= 1.0f, "min depth must be between 0 and 1");
    ionAssertReturnVoid(_maxDepth >= 0.0f && _maxDepth <= 1.0f, "max depth must be between 0 and 1");

    _renderCore.SetViewport(static_cast<ionFloat>(_fromX), static_cast<ionFloat>(_fromY), static_cast<ionFloat>(_width)  * _percentageOfWithHeight, static_cast<ionFloat>(_height)  * _percentageOfWithHeight, _minDepth, _maxDepth);
}

void Camera::SetScissor(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight)
{
    ionAssertReturnVoid(_percentageOfWithHeight >= 0.0f && _percentageOfWithHeight <= 1.0f, "pergentage must be between 0 and 1");

    _renderCore.SetScissor(_fromX, _fromY, static_cast<ionU32>((static_cast<ionFloat>(_width)  * _percentageOfWithHeight)), static_cast<ionU32>((static_cast<ionFloat>(_height)  * _percentageOfWithHeight)));
}

void Camera::StartRenderPass(RenderCore& _renderCore, ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue)
{
    ionAssertReturnVoid(_clearDepthValue >= 0.0f && _clearDepthValue <= 1.0f, "Clear depth must be between 0 and 1!");
    ionAssertReturnVoid(_clearRed >= 0.0f && _clearRed <= 1.0f, "Clear red must be between 0 and 1!");
    ionAssertReturnVoid(_clearGreen >= 0.0f && _clearGreen <= 1.0f, "Clear green must be between 0 and 1!");
    ionAssertReturnVoid(_clearBlue >= 0.0f && _clearBlue <= 1.0f, "Clear blue must be between 0 and 1!");

    _renderCore.StartRenderPass(_clearDepthValue, _clearStencilValue, _clearRed, _clearGreen, _clearBlue);
}

void Camera::EndRenderPass(RenderCore& _renderCore)
{
    _renderCore.EndRenderPass();
}


ION_NAMESPACE_END