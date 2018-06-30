#include "Objects.h"


//////////////////////////////////////////////////////////////////////////
// ENTITIES

void RotatingEntity::OnUpdate(ionFloat _deltaTime)
{
    static const ionFloat radPerFrame = 0.0174533f;     // 1 deg
    static const Vector axis(0.0f, 1.0f, 0.0f, 1.0f);
    static ionFloat radRotated = 0.0f;

    radRotated += radPerFrame;
    while (radRotated > 6.283185307f) radRotated -= 6.283185307f;   // 360 deg

    GetTransformHandle()->SetRotation(radRotated, axis);
}


//////////////////////////////////////////////////////////////////////////
// CAMERA

FPSCamera::FPSCamera() : Camera("FPS Camera"), m_movementSpeed(1.0f), m_mouseSensitivity(0.05f), m_pitchDeg(0.0f), m_yawDeg(0.0f), m_constrainPitch(true)
{

}

FPSCamera::~FPSCamera()
{

}

void FPSCamera::SetParameters(ionFloat _movementSpeed, ionFloat _mouseSensitivity, ionBool _constrainPitch)
{
    m_movementSpeed = _movementSpeed;
    m_mouseSensitivity = _mouseSensitivity;
    m_constrainPitch = _constrainPitch;
}

void FPSCamera::ProcessMouseMovement(ionFloat _xOffset, ionFloat _yOffset, ionFloat _xAbs, ionFloat _yAbs)
{
    ION_UNUSED(_xAbs);
    ION_UNUSED(_yAbs);

    _xOffset *= m_mouseSensitivity;
    _yOffset *= m_mouseSensitivity;

    m_yawDeg += _xOffset;
    m_pitchDeg -= _yOffset;

    if (m_constrainPitch)
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

    Matrix rotationMatrix;
    rotationMatrix.SetFromYawPitchRoll(NIX_DEG_TO_RAD(m_yawDeg), NIX_DEG_TO_RAD(m_pitchDeg), NIX_DEG_TO_RAD(0.0f));

    Quaternion rotation;
    rotation.SetFromMatrix(rotationMatrix);

    GetTransformHandle()->SetRotation(rotation);
}

