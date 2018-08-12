#include "Objects.h"



//////////////////////////////////////////////////////////////////////////
// ENTITIES

RotatingEntity::RotatingEntity() : m_rotating(false)
{

}

RotatingEntity::RotatingEntity(const eosString & _name) : Entity(_name), m_rotating(false)
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

FPSCamera::FPSCamera() : Camera("FPS Camera"), m_movementSpeed(1.0f), m_mouseSensitivity(0.05f), m_pitchDeg(0.0f), m_yawDeg(0.0f), m_constrainPitch(true)
{
#ifdef ION_PBR_DEBUG
    m_pbrDebug = EPBRDebugType_Exposure;
    std::cout << std::endl << "Active PBR Debug: Esposure - Current level is " << ionRenderManager().m_exposure << std::endl <<
        "Press 1, 2 or 3 to change the active value " << std::endl << "[esposure, gamma or prefiltered cube mip levels]" << std::endl <<
        "and then press Q or Z to increment or decrement such values." << std::endl <<
        "This works only with the PBR test (number 7)" << std::endl << std::endl;
#endif
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

void FPSCamera::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
    ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    m_yawDeg += xOffset;
    m_pitchDeg -= yOffset;

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

    GetTransform().SetRotation(rotation);
}

void FPSCamera::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    static const Vector right(1.0f, 0.0f, 0.0f, 0.0f);
    static const Vector forward(0.0f, 0.0f, 1.0f, 0.0f);

    ionFloat velocity = m_movementSpeed * _deltaTime;

    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        Vector pos = GetTransform().GetPosition();

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

#ifdef ION_PBR_DEBUG
        else if (_keyboardState.m_key == ion::EKeyboardKey_Q)
        {
            switch (m_pbrDebug)
            {
            case FPSCamera::EPBRDebugType_Exposure:
                ionRenderManager().m_exposure += 0.01f;
                std::cout << "Incremented to " << ionRenderManager().m_exposure << std::endl;
                break;
            case FPSCamera::EPBRDebugType_Gamma:
                ionRenderManager().m_gamma += 0.01f;
                std::cout << "Incremented to " << ionRenderManager().m_gamma << std::endl;
                break;
            case FPSCamera::EPBRDebugType_PrefilteredCubeMipLevels:
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
            case FPSCamera::EPBRDebugType_Exposure:
                ionRenderManager().m_exposure -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_exposure << std::endl;
                break;
            case FPSCamera::EPBRDebugType_Gamma:
                ionRenderManager().m_gamma -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_gamma << std::endl;
                break;
            case FPSCamera::EPBRDebugType_PrefilteredCubeMipLevels:
                ionRenderManager().m_prefilteredCubeMipLevels -= 0.01f;
                std::cout << "Decremented to " << ionRenderManager().m_prefilteredCubeMipLevels << std::endl;
                break;
            default:
                break;
            }
        }
#endif

        GetTransform().SetPosition(pos); 
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

