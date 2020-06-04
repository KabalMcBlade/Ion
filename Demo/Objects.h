#pragma once

#include "../Ion/Ion.h"

//////////////////////////////////////////////////////////////////////////
// ENTITIES

class DirectionalLightDebugEntity : public ion::Entity
{
public:
    DirectionalLightDebugEntity();
    virtual ~DirectionalLightDebugEntity();

    virtual void OnUpdate(ionFloat _deltaTime) override;

    virtual void OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime) override;

private:
    ionFloat m_mouseSensitivity;
};

//////////////////////////////////////////////////////////////////////////

class MainCamera;
class RotatingEntity : public ion::Entity
{
public:
    RotatingEntity();
    RotatingEntity(const ion::ionString& _name);
    virtual ~RotatingEntity();

    virtual void OnBegin() override;
    virtual void OnEnd() override;

    virtual void OnUpdate(ionFloat _deltaTime) override;

    virtual void OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime) override;
    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

    void SetCameraReference(MainCamera* _camera);

	void StoreDefaultPosRotScale();

private:
	Quaternion m_defaultRot;
	Vector4 m_defaultPos;
	Vector4 m_defaultScale;

    ionU32 m_animationCount;
    ionU32 m_currentAnimationIndex;

    ionFloat m_animationSpeedMultiplier;
    ionFloat m_movementSpeed;
	Scalar m_mouseSensitivity;
    ionFloat m_incresingWheelSpeed;

    MainCamera* m_camera;

    ionBool m_rotating;
};


//////////////////////////////////////////////////////////////////////////
// CAMERA
class MainCamera : public ion::Camera
{
public:
    MainCamera();
    virtual ~MainCamera();

public:
    virtual void OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime) override;

private:
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;
    
public:
	static ionBool m_toggleLightRotation;

private:
    enum EPBRDebugType
    {
        EPBRDebugType_Exposure = 0,
        EPBRDebugType_Gamma,
        EPBRDebugType_PrefilteredCubeMipLevels
    };
    EPBRDebugType m_pbrDebug;
};