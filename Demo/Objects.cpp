#include "Objects.h"



//////////////////////////////////////////////////////////////////////////
// ENTITIES

RotatingEntity::RotatingEntity() : m_rotating(false), m_mouseSensitivity(0.05f)
{

}

RotatingEntity::RotatingEntity(const eosString & _name) : Entity(_name), m_rotating(false), m_mouseSensitivity(0.05f)
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
    }
}


void RotatingEntity::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
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
}


//////////////////////////////////////////////////////////////////////////

BoundingBoxEntity::BoundingBoxEntity() : m_waitForAction(false)
{
}

BoundingBoxEntity::BoundingBoxEntity(const eosString & _name) : Entity(_name), m_waitForAction(false)
{
}

BoundingBoxEntity::~BoundingBoxEntity()
{
}

void BoundingBoxEntity::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_B)
        {
            if (!m_waitForAction)
            {
                if (IsVisible())
                {
                    SetVisible(false);
                }
                else
                {
                    SetVisible(true);
                }
                m_waitForAction = true;
            }
        }
    }
    else if (_keyboardState.m_state == ion::EKeyboardState_Up)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_B)
        {
            m_waitForAction = false;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// CAMERA

MainCamera::MainCamera() : Camera("FPS Camera"), m_movementSpeed(1.0f), m_mouseSensitivity(0.05f)
{
#ifdef ION_PBR_DEBUG
    m_pbrDebug = EPBRDebugType_Exposure;
    std::cout << std::endl << "Active PBR Debug: Esposure - Current level is " << ionRenderManager().m_exposure << std::endl <<
        "Press 1, 2 or 3 to change the active value " << std::endl << "[esposure, gamma or prefiltered cube mip levels]" << std::endl <<
        "and then press Q or Z to increment or decrement such values." << std::endl <<
        "This works only with the PBR test (number 7)" << std::endl << std::endl;
#endif
}

MainCamera::~MainCamera()
{

}

void MainCamera::SetParameters(ionFloat _movementSpeed, ionFloat _mouseSensitivity)
{
    m_movementSpeed = _movementSpeed;
    m_mouseSensitivity = _mouseSensitivity;
}

void MainCamera::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    if (_mouseState.m_wheel.m_wasMoved)
    {
        static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);

        const Quaternion& orientation = GetTransform().GetRotation();

        ionFloat velocity = m_movementSpeed * _deltaTime;

        Vector pos = GetTransform().GetPosition();

        pos = pos * orientation;

        pos += forward * _mouseState.m_wheel.m_distance * velocity;

        GetTransform().SetPosition(pos);
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

        const Quaternion& orientation = GetTransform().GetRotation();

        Vector dir = right * xOffset + up * yOffset;
        dir = dir * orientation;

        Vector pos = GetTransform().GetPosition();

        pos += dir * velocity;

        GetTransform().SetPosition(pos);
    }
}

void MainCamera::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
#ifdef ION_PBR_DEBUG
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
#endif
    }

    if (_keyboardState.m_state == ion::EKeyboardState_Up)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_Escape)
        {
            ionRenderManager().Quit();
        }

#ifdef ION_PBR_DEBUG
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
#endif
    }
}

