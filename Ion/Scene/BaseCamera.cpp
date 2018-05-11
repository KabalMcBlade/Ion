#include "BaseCamera.h"


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

Matrix BaseCamera::PerspectiveProjectionMatrix(ionFloat _fovDeg, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar)
{
    _fovDeg *= NIX_VALUE_DEGTORAD;  //  this is PI / 180.0;

    ionFloat field = 1.0f / tanf(0.5f * _fovDeg);

    Matrix perspective(
        field / _aspect,    0.0f,       0.0f,                                   0.0f,
        0.0f,               field,      0.0f,                                   0.0f,
        0.0f,               0.0f,       (_zFar + _zNear) / (_zNear - _zFar),    (2.0f * _zFar * _zNear) / (_zNear - _zFar),
        0.0f,               0.0f,       -1.0f,                                  0.0f
    );

    return perspective;
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



ION_NAMESPACE_END