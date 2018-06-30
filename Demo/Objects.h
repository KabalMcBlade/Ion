#pragma once

#include "../Ion/Ion.h"

//////////////////////////////////////////////////////////////////////////
// ENTITIES

class RotatingEntity : public ion::Entity
{
    virtual void OnUpdate(ionFloat _deltaTime) override;
};


//////////////////////////////////////////////////////////////////////////
// CAMERA
class FPSCamera : public ion::Camera
{
public:
    FPSCamera();
    virtual ~FPSCamera();

    void SetParameters(ionFloat _movementSpeed, ionFloat _mouseSensitivity, ionBool _constrainPitch);

public:
    virtual void ProcessMouseMovement(ionFloat _xOffset, ionFloat _yOffset, ionFloat _xAbs, ionFloat _yAbs) override;

private:
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;

    ionFloat m_pitchDeg;
    ionFloat m_yawDeg;

    ionBool m_constrainPitch;
};