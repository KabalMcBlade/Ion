#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"
#include "../Geometry/Frustum.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Camera;
typedef SmartPointer<Camera> CameraHandle;

class RenderCore;
class ION_DLL Camera : public Node
{
public:
    enum ECameraType 
    { 
        ECameraType_LookAt = 0,
        ECameraType_FirstPerson
    };
    
    explicit Camera();
    explicit Camera(const eosString & _name);
    virtual ~Camera();

    void SetCameraType(ECameraType _type);
    void SetPerspectiveProjection(ionFloat _fovDeg, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar);
    void UpdateAspectRatio(ionFloat _aspect);

    void UpdateView();

    ionFloat GetNear() const { return m_zNear; }
    ionFloat GetFar() const { return m_zFar; }
    ionFloat GetFovDeg() const { return NIX_RAD_TO_DEG(m_fov); }
    ionFloat GetFovRad() const { return m_fov; }

    void ProcessMouseMovement(ionFloat _xOffset, ionFloat _yOffset, ionBool _constrainPitch = true);

    const Matrix& GetPerspectiveProjection() const { return m_projection; }
    const Matrix& GetView() const { return m_view; }

    const Frustum& GetFrustum() const { return m_frustum; }

    void SetViewport(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight, ionFloat _minDepth, ionFloat _maxDepth);
    void SetScissor(RenderCore& _renderCore, ionS32 _fromX, ionS32 _fromY, ionS32 _width, ionS32 _height, ionFloat _percentageOfWithHeight);
    void StartRenderPass(RenderCore& _renderCore, ionFloat _clearDepthValue, ionU8 _clearStencilValue, ionFloat _clearRed, ionFloat _clearGreen, ionFloat _clearBlue);
    void EndRenderPass(RenderCore& _renderCore);

public:
    static Matrix PerspectiveProjectionMatrix(ionFloat _fov, ionFloat _aspect, ionFloat _zNear, ionFloat _zFar);
    static Matrix OrthographicProjectionMatrix(ionFloat _left, ionFloat _right, ionFloat _bottom, ionFloat _top, ionFloat _zNear, ionFloat _zFar);

private:
    Camera(const Camera& _Orig) = delete;
    Camera& operator = (const Camera&) = delete;

private:
    Matrix  m_projection;
    Matrix  m_view;

    Frustum m_frustum;

    ionFloat m_fov;
    ionFloat m_zNear;
    ionFloat m_zFar;

    //////////////////////////////////////////////////////////////////////////
    // in an inherited camera?
    //////////////////////////////////////////////////////////////////////////
    ionFloat m_movementSpeed;
    ionFloat m_mouseSensitivity;

    ionFloat m_pitchDeg;
    ionFloat m_yawDeg;
    //////////////////////////////////////////////////////////////////////////

    ECameraType m_type;
};

ION_NAMESPACE_END