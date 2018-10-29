#include "Objects.h"



ionBool MainCamera::m_toggleLightRotation = false;

//////////////////////////////////////////////////////////////////////////
// ENTITIES


DirectionalLightDebugEntity::DirectionalLightDebugEntity() : Entity("DirectionalLightDebugEntity"), m_mouseSensitivity(0.05f)
{

}

DirectionalLightDebugEntity::~DirectionalLightDebugEntity()
{

}

void DirectionalLightDebugEntity::OnUpdate(ionFloat _deltaTime)
{
    if (MainCamera::m_toggleLightRotation != IsVisible())
    {
        SetVisible(MainCamera::m_toggleLightRotation);
    }
}

void DirectionalLightDebugEntity::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    if (MainCamera::m_toggleLightRotation)
    {
        ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
        ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        const Quaternion& prevRot = GetTransform().GetRotation();

        Quaternion currRot(NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(xOffset), 0.0f);

        currRot = currRot * prevRot;

        GetTransform().SetRotation(currRot);
    }
}


//////////////////////////////////////////////////////////////////////////


RotatingEntity::RotatingEntity() : m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f)
{

}

RotatingEntity::RotatingEntity(const eosString & _name) : Entity(_name), m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f)
{
}


RotatingEntity::~RotatingEntity()
{

}

void RotatingEntity::SetCameraReference(MainCamera* _camera)
{
    m_camera = _camera;
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
    if (!MainCamera::m_toggleLightRotation)
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
}

void RotatingEntity::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
    static const Vector up(0.0f, 1.0f, 0.0f, 0.0f);
    static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);

    if (!MainCamera::m_toggleLightRotation)
    {
        if (_mouseState.m_buttons[0].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = GetTransform().GetRotation();

            Quaternion currRot(NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(xOffset), 0.0f);

            currRot = currRot * prevRot;

            GetTransform().SetRotation(currRot);
        }

        if (_mouseState.m_buttons[1].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            ionFloat velocity = m_movementSpeed * _deltaTime;

            const Quaternion& orientation = m_camera->GetTransform().GetRotation();
            const Vector newRight =  orientation * right;
            const Vector newUp = orientation * up;

            const Vector dir = newRight * xOffset + newUp * yOffset;

            const Vector pos = GetTransform().GetPosition() + dir * velocity;

            GetTransform().SetPosition(pos);
        }

        if (_mouseState.m_wheel.m_wasMoved)
        {
            ionFloat velocity = m_movementSpeed * m_incresingWheelSpeed * _deltaTime;

            const Quaternion& orientation = m_camera->GetTransform().GetRotation();
            const Vector newForward = forward * orientation;

            const Vector dir = newForward * _mouseState.m_wheel.m_distance;

            const Vector pos = GetTransform().GetPosition() + dir * velocity;

            GetTransform().SetPosition(pos);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// CAMERA

MainCamera::MainCamera() : Camera("Main Camera"), m_mouseSensitivity(0.05f), m_movementSpeed(0.001f), m_mouseNotUsed(true)
{
    m_pbrDebug = EPBRDebugType_Exposure;
    std::cout << std::endl << "Control applied to the object" << std::endl;
}

MainCamera::~MainCamera()
{
}

void MainCamera::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    m_mouseNotUsed = !_mouseState.m_buttons[0].IsPressed && !_mouseState.m_buttons[1].IsPressed && !_mouseState.m_wheel.m_wasMoved;

    if (MainCamera::m_toggleLightRotation)
    {
        ion::DirectionalLight* directionalLight = ionRenderManager().GetDirectionalLight();

        ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
        ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        const Quaternion& prevRot = directionalLight->GetTransform().GetRotation();

        Quaternion currRot(NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(xOffset), 0.0f);

        currRot = currRot * prevRot;

        directionalLight->GetTransform().SetRotation(currRot);
    }
    else
    {
        if (m_mouseNotUsed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = GetTransform().GetRotation();

            Quaternion currRot(NIX_DEG_TO_RAD(-yOffset), NIX_DEG_TO_RAD(xOffset), 0.0f);

            currRot = currRot * prevRot;

            GetTransform().SetRotation(currRot);
        }
    }
}

void MainCamera::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        if (m_mouseNotUsed)
        {
            static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
            static const Vector up(0.0f, 1.0f, 0.0f, 0.0f);
            static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);

            Vector pos = GetTransform().GetPosition();

            ionFloat velocity = m_movementSpeed * _deltaTime;


            if (_keyboardState.m_key == ion::EKeyboardKey_W)
            {
                pos += forward * velocity;
            }
            else if (_keyboardState.m_key == ion::EKeyboardKey_S)
            {
                pos -= forward * velocity;
            }
            else if (_keyboardState.m_key == ion::EKeyboardKey_D)
            {
                pos -= right * velocity;
            }
            else if (_keyboardState.m_key == ion::EKeyboardKey_A)
            {
                pos += right * velocity;
            }

            GetTransform().SetPosition(pos);
        }

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

