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

		Quaternion currRotX;
		currRotX.SetFromAngleAxis(NIX_DEG_TO_RAD(xOffset), 0.0f, 0.0f, -1.0f);
		Quaternion currRotY;
		currRotY.SetFromAngleAxis(NIX_DEG_TO_RAD(yOffset), -1.0f, 0.0f, 0.0f);

		Quaternion currRot = prevRot * currRotY * currRotX;

        GetTransform().SetRotation(currRot);
		ionRenderManager().GetDirectionalLight()->GetTransform().SetRotation(currRot);
    }
}


//////////////////////////////////////////////////////////////////////////


RotatingEntity::RotatingEntity() : m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f), m_animationCount(0), m_currentAnimationIndex(0), m_animationSpeedMultiplier(0.25f)
{

};

RotatingEntity::RotatingEntity(const ion::ionString & _name) : Entity(_name), m_rotating(false), m_mouseSensitivity(0.05f), m_movementSpeed(10.0f), m_incresingWheelSpeed(1.0f), m_animationCount(0), m_currentAnimationIndex(0), m_animationSpeedMultiplier(0.25f)
{
}


RotatingEntity::~RotatingEntity()
{

}

void RotatingEntity::SetCameraReference(MainCamera* _camera)
{
    m_camera = _camera;
}

void RotatingEntity::OnBegin()
{
    ion::AnimationRenderer* animationRenderer = GetAnimationRenderer();
    if (animationRenderer != nullptr)
    {
        if (animationRenderer->IsEnabled())
        {
            m_animationCount = static_cast<ionU32>(animationRenderer->GetAnimationCount());
            if (m_animationCount > 0)
            {
                std::cout << "Model has " << m_animationCount << " animation/s" << std::endl;

                animationRenderer->SetAnimationSpeedMultiplier(m_animationSpeedMultiplier);
                animationRenderer->SetAnimationToPlay(m_currentAnimationIndex);

                std::cout << "Play animation " << m_currentAnimationIndex << std::endl;
            }
        }
    }
}

void RotatingEntity::OnEnd()
{

}

void RotatingEntity::OnUpdate(ionFloat _deltaTime)
{
    static const ionFloat radPerFrame = 0.0174533f;     // 1 deg
    static const Vector4 axis(0.0f, 1.0f, 0.0f, 1.0f);
    static ionFloat radRotated = 0.0f;

    if (m_rotating)
    {
        const Quaternion& prevRot = GetTransform().GetRotation();

        Quaternion currRot = Quaternion(radPerFrame, axis);
        currRot = prevRot * currRot;

        GetTransform().SetRotation(currRot);
    }
}

void RotatingEntity::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (!MainCamera::m_toggleLightRotation)
    {
        if (_keyboardState.m_state == ion::EKeyboardState_Up)
        {
            if (_keyboardState.m_key == ion::EKeyboardKey_Return)
            {
                // Reset the object matrix
                GetTransform().SetScale(1.0f);

                Quaternion qIdentity;
                GetTransform().SetRotation(qIdentity);

                GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_R)
            {
                m_rotating = !m_rotating;
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_J || _keyboardState.m_key == ion::EKeyboardKey_K)
            {
                m_incresingWheelSpeed = 1.0f;
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_I)
            {
                Vector4 scale = GetTransform().GetScale() / 10.0f;
                GetTransform().SetScale(scale);
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_O)
            {
                Vector4 scale = GetTransform().GetScale() * 10.0f;
                GetTransform().SetScale(scale);
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_Up && m_animationCount > 0)
            {
                ion::AnimationRenderer* animationRenderer = GetAnimationRenderer();
                if (animationRenderer != nullptr)
                {
                    if (animationRenderer->IsEnabled())
                    {
                        ++m_currentAnimationIndex;
                        m_currentAnimationIndex %= m_animationCount;

                        animationRenderer->SetAnimationToPlay(m_currentAnimationIndex);

                        std::cout << "Play animation " << m_currentAnimationIndex << std::endl;
                    }
                }
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_Down && m_animationCount > 0)
            {
                ion::AnimationRenderer* animationRenderer = GetAnimationRenderer();
                if (animationRenderer != nullptr)
                {
                    if (animationRenderer->IsEnabled())
                    {
                        --m_currentAnimationIndex;
                        m_currentAnimationIndex %= m_animationCount;

                        animationRenderer->SetAnimationToPlay(m_currentAnimationIndex);

                        std::cout << "Play animation " << m_currentAnimationIndex << std::endl;
                    }
                }
            }
        }

        if (_keyboardState.m_state == ion::EKeyboardState_Down)
        {
            if (_keyboardState.m_key == ion::EKeyboardKey_J)
            {
                m_incresingWheelSpeed = 10.0f;
            }
            if (_keyboardState.m_key == ion::EKeyboardKey_K)
            {
                m_incresingWheelSpeed = 100.0f;
            }

            if (_keyboardState.m_key == ion::EKeyboardKey_Right && m_animationCount > 0)
            {
                ion::AnimationRenderer* animationRenderer = GetAnimationRenderer();
                if (animationRenderer != nullptr)
                {
                    if (animationRenderer->IsEnabled())
                    {
                        m_animationSpeedMultiplier += 0.00001f;

                        animationRenderer->SetAnimationSpeedMultiplier(m_animationSpeedMultiplier);
                    }
                }
            }
            if (_keyboardState.m_key == ion::EKeyboardKey_Left && m_animationCount > 0)
            {
                ion::AnimationRenderer* animationRenderer = GetAnimationRenderer();
                if (animationRenderer != nullptr)
                {
                    if (animationRenderer->IsEnabled())
                    {
                        m_animationSpeedMultiplier -= 0.00001f;

                        animationRenderer->SetAnimationSpeedMultiplier(m_animationSpeedMultiplier);
                    }
                }
            }

        }
    }
}

void RotatingEntity::OnMouseInput(const ion::MouseState& _mouseState, ionFloat _deltaTime)
{
    static const Vector4 right(1.0f, 0.0f, 0.0f, 1.0f);
    static const Vector4 up(0.0f, 1.0f, 0.0f, 1.0f);
    static const Vector4 forward(0.0f, 0.0f, 1.0f, 1.0f);

    if (!MainCamera::m_toggleLightRotation)
    {
        if (_mouseState.m_buttons[0].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            const Quaternion& prevRot = GetTransform().GetRotation();

			Quaternion currRotX;
			currRotX.SetFromAngleAxis(NIX_DEG_TO_RAD(xOffset), 0.0f, 0.0f, -1.0f);
			Quaternion currRotY;
			currRotY.SetFromAngleAxis(NIX_DEG_TO_RAD(yOffset), -1.0f, 0.0f, 0.0f);

			Quaternion currRot = prevRot * currRotY * currRotX;

            GetTransform().SetRotation(currRot);
        }

        if (_mouseState.m_buttons[1].IsPressed)
        {
            ionFloat xOffset = _mouseState.m_position.m_delta.m_x;
            ionFloat yOffset = _mouseState.m_position.m_delta.m_y;

            xOffset *= m_mouseSensitivity;
            yOffset *= m_mouseSensitivity;

            ionFloat velocity = m_movementSpeed * _deltaTime;

            const Matrix4x4& matrix = m_camera->GetTransform().GetMatrixWS();
            Vector4 right = matrix.GetOrtX();
            Vector4 up = matrix.GetOrtY();

            const Vector4 dir = right * xOffset + up * yOffset;

            const Vector4 pos = GetTransform().GetPosition() + dir * velocity;

            GetTransform().SetPosition(pos);
        }

        if (_mouseState.m_wheel.m_wasMoved)
        {
            ionFloat velocity = m_movementSpeed * m_incresingWheelSpeed * _deltaTime;

            const Matrix4x4& matrix = m_camera->GetTransform().GetMatrixWS();
            Vector4 forward = matrix.GetOrtZ();

            const Vector4 dir = forward * _mouseState.m_wheel.m_distance;

            const Vector4 pos = GetTransform().GetPosition() + dir * velocity;

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

void MainCamera::OnKeyboardInput(const ion::KeyboardState& _keyboardState, ionFloat _deltaTime)
{
    if (_keyboardState.m_state == ion::EKeyboardState_Down)
    {
        if (_keyboardState.m_key == ion::EKeyboardKey_Prior)
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
        else if (_keyboardState.m_key == ion::EKeyboardKey_Next)
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
        else if (_keyboardState.m_key == ion::EKeyboardKey_E)
        {
            m_pbrDebug = EPBRDebugType_Exposure;
            std::cout << "Active PBR Debug: Esposure - Current level is " << ionRenderManager().m_exposure << std::endl;
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_G)
        {
            m_pbrDebug = EPBRDebugType_Gamma;
            std::cout << "Active PBR Debug: Gamma - Current level is " << ionRenderManager().m_gamma << std::endl;
        }
        else if (_keyboardState.m_key == ion::EKeyboardKey_P)
        {
            m_pbrDebug = EPBRDebugType_PrefilteredCubeMipLevels;
            std::cout << "Active PBR Debug: Prefiltered Cube Mip Levels - Current level is " << ionRenderManager().m_prefilteredCubeMipLevels << std::endl;
        }
    }
}

