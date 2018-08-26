#pragma once

#include "../Ion/Ion.h"

//////////////////////////////////////////////////////////////////////////
// ENTITIES

class RotatingEntity : public ion::Entity
{
public:
    RotatingEntity();
    RotatingEntity(const eosString & _name);
    virtual ~RotatingEntity();

    virtual void OnUpdate(ionFloat _deltaTime) override;

    virtual void OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime) override;
    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;
    ionFloat m_scale;

    ionBool m_rotating;
};


class BoundingBoxEntity : public ion::Entity
{
public:
    BoundingBoxEntity();
    BoundingBoxEntity(const eosString & _name);
    virtual ~BoundingBoxEntity();

    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionBool m_waitForAction;
};

//////////////////////////////////////////////////////////////////////////
// CAMERA
class MainCamera : public ion::Camera
{
public:
    MainCamera();
    virtual ~MainCamera();

public:
    virtual void OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime) override;
    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;
    
public:
    static ionBool m_toggleLightRotation;

#ifdef ION_PBR_DEBUG
private:
    enum EPBRDebugType
    {
        EPBRDebugType_Exposure = 0,
        EPBRDebugType_Gamma,
        EPBRDebugType_PrefilteredCubeMipLevels
    };
    EPBRDebugType m_pbrDebug;
#endif
};