#include "Objects.h"



ionBool MainCamera::m_toggleLightRotation = false;

//////////////////////////////////////////////////////////////////////////
// ENTITIES

RotatingEntity::RotatingEntity() : m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f)
{

}

RotatingEntity::RotatingEntity(const eosString & _name) : Entity(_name), m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f)
{
}


RotatingEntity::~RotatingEntity()
{

}

void RotatingEntity::OnUpdate(ionFloat _deltaTime)
{
    static const ionFloat radPerFrame = 0.0174533f;     // 1 deg
    static const Vector axis(0.0f, 1.0f, 0.0f, 0.0f);
    static ionFloat radRotated = 0.0f;

    if (m_rotating)
    {
        const Quaternion& prevRot = GetTransform().GetRotation();

        Quaternion currRot = Quaternion(radPerFrame, axis);
        currRot = currRot * prevRot;

        GetTransform().SetRotation(currRot);
    }
}

void RotatingEntity::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Up)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_R)
        {
            m_rotating = !m_rotating;
        }

        if (_keyboardState.m_key == ion::EKeyboardKey_G || _keyboardState.m_key == ion::EKeyboardKey_H)
        {
            m_incresingWheelSpeed = 1.0f;
        }

        if (_keyboardState.m_key == ion::EKeyboardKey_U)
        {
            Vector scale = GetTransform().GetScale() / 10.0f;
            GetTransform().SetScale(scale);
        }

        if (_keyboardState.m_key == ion::EKeyboardKey_I)
        {
            Vector scale = GetTransform().GetScale() * 10.0f;
            GetTransform().SetScale(scale);
        }
    }

    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_G)
        {
            m_incresingWheelSpeed = 10.0f;
        }
        if (_keyboardState.m_key == ion::EKeyboardKey_H)
        {
            m_incresingWheelSpeed = 100.0f;
        }
    }
}

void RotatingEntity::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    if (!MainCamera::m_toggleLightRotation)
    {
        if (_mouseState.m_buttons[0].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = GetTransform().GetRotation();

            Matrix rotationMatrix;
            rotationMatrix.SetFromYawPitchRoll(NIX_DEG_TO_RAD(xOffset), NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(0.0f));

            Quaternion currRot;
            currRot.SetFromMatrix(rotationMatrix);

            currRot = currRot * prevRot;

            GetTransform().SetRotation(currRot);
        }

        if (_mouseState.m_buttons[1].IsPressed)
        {
            static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
            static const Vector up(0.0f, 1.0f, 0.0f, 0.0f);

            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            ionFloat velocity = m_movementSpeed * _deltaTime;

            Vector dir = right * xOffset + up * yOffset;
            Vector pos = GetTransform().GetPosition();

            pos += dir * velocity;

            GetTransform().SetPosition(pos);
        }

        if (_mouseState.m_wheel.m_wasMoved)
        {
            static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);

            ionFloat velocity = m_movementSpeed * m_incresingWheelSpeed * _deltaTime;

            Vector dir = forward * _mouseState.m_wheel.m_distance;
            Vector pos = GetTransform().GetPosition();

            pos += dir * velocity;

            GetTransform().SetPosition(pos);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// CAMERA

MainCamera::MainCamera() : Camera("Main Camera"), m_mouseSensitivity(0.05f), m_movementSpeed(0.001f)
{
    m_pbrDebug = EPBRDebugType_Exposure;
    std::cout << std::endl << "Control applied to the object" << std::endl;
}

MainCamera::~MainCamera()
{
}

void MainCamera::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    if (MainCamera::m_toggleLightRotation)
    {
        ion::DirectionalLight* directionalLight = ionRenderManager().GetDirectionalLight();

        if (_mouseState.m_buttons[0].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = directionalLight->GetTransform().GetRotation();

            Matrix rotationMatrix;
            rotationMatrix.SetFromYawPitchRoll(NIX_DEG_TO_RAD(xOffset), NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(0.0f));

            Quaternion currRot;
            currRot.SetFromMatrix(rotationMatrix);

            currRot = currRot * prevRot;

            directionalLight->GetTransform().SetRotation(currRot);
        }
    }
    else
    {
        if (!_mouseState.m_buttons[0].IsPressed && !_mouseState.m_buttons[1].IsPressed && !_mouseState.m_wheel.m_wasMoved)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = GetTransform().GetRotation();

            Matrix rotationMatrix;
            rotationMatrix.SetFromYawPitchRoll(NIX_DEG_TO_RAD(xOffset), NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(0.0f));

            Quaternion currRot;
            currRot.SetFromMatrix(rotationMatrix);

            currRot = currRot * prevRot;

            GetTransform().SetRotation(currRot);
        }
    }
}

void MainCamera::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_Q)
        {
            switch (m_pbrDebug)
            {
            case MainCamera::EPBRDebugType_Exposure:
                ionRenderManager().m_exposure += 0.01f;
                std::cout << "Incremented to " << ionRenderManager().m_exposure << std::endl;
                break;
            case MainCamera::EPBRDebugType_Gamma:
                ionRenderManager().m_gamma += 0.01f;
                std::cout << "Incremented to " << ionRenderManager().m_gamma << std::endl;
                break;
            case MainCamera::EPBRDebugType_PrefilteredCubeMipLevels:
                ionRenderManager().m_prefilteredCubeMipLevels += 0.01f;
                std::cout << "Incremented to " << ionRenderManager().m_prefilteredCubeMipLevels << std::endl;
                break;
            default:
                break;
            }
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_Z)
        {
            switch (m_pbrDebug)
            {
            case MainCamera::EPBRDebugType_Exposure:
                ionRenderManager().m_exposure -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_exposure << std::endl;
                break;
            case MainCamera::EPBRDebugType_Gamma:
                ionRenderManager().m_gamma -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_gamma << std::endl;
                break;
            case MainCamera::EPBRDebugType_PrefilteredCubeMipLevels:
                ionRenderManager().m_prefilteredCubeMipLevels -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_prefilteredCubeMipLevels << std::endl;
                break;
            default:
                break;
            }
        }
    }

    if (_keyboardState.m_state == ion::EKeyboardState_Up)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_Escape)
        {
            ionRenderManager().Quit();
        }

        if (_keyboardState.m_key == ion::EKeyboardKey_L)
        {
            MainCamera::m_toggleLightRotation = !MainCamera::m_toggleLightRotation;

            if (MainCamera::m_toggleLightRotation)
            {
                std::cout << "Control applied to the light" << std::endl;
            }
            else
            {
                std::cout << "Control applied to the object" << std::endl;
            }
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_N1)
        {
            m_pbrDebug = EPBRDebugType_Exposure;
            std::cout << "Active PBR Debug: Esposure - Current level is " << ionRenderManager().m_exposure << std::endl;
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_N2)
        {
            m_pbrDebug = EPBRDebugType_Gamma;
            std::cout << "Active PBR Debug: Gamma - Current level is " << ionRenderManager().m_gamma << std::endl;
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_N3)
        {
            m_pbrDebug = EPBRDebugType_PrefilteredCubeMipLevels;
            std::cout << "Active PBR Debug: Prefiltered Cube Mip Levels - Current level is " << ionRenderManager().m_prefilteredCubeMipLevels << std::endl;
        }
    }
}

