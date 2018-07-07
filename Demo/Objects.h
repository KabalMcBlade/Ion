#pragma once

#include "../Ion/Ion.h"

//////////////////////////////////////////////////////////////////////////
// ENTITIES

class RotatingEntity : public ion::Entity
{
public:
    RotatingEntity();
    virtual ~RotatingEntity();

    virtual void OnUpdate(ionFloat _deltaTime) override;

    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionBool m_rotating;
};


class BoundingBoxEntity : public ion::Entity
{
public:
    BoundingBoxEntity();
    virtual ~BoundingBoxEntity();

    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionBool m_waitForAction;
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
    virtual void OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime) override;
    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;

    ionFloat m_pitchDeg;
    ionFloat m_yawDeg;

    ionBool m_constrainPitch;
};