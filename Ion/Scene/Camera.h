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

    const Matrix& GetPerspectiveProjection() const { return m_projection; }
    const Matrix& GetView() const { return m_view; }

    const Frustum& GetFrustum() const { return m_frustum; }

    //////////////////////////////////////////////////////////////////////////
    // Setting

    // The parameters are normalized depending from the screen
    void SetViewportParameters(ionFloat _x = 0.0f, ionFloat _y = 0.0f, ionFloat _width = 1.0f, ionFloat _height = 1.0f, ionFloat _minDepth = 0.0f, ionFloat _maxDepth = 1.0f);
    void SetScissorParameters(ionFloat _x = 0.0f, ionFloat _y = 0.0f, ionFloat _width = 1.0f, ionFloat _height = 1.0f);
    void SetRenderPassParameters(ionFloat _clearDepthValue = 1.0f, ionU8 _clearStencilValue = 0, ionFloat _clearRed = 1.0f, ionFloat _clearGreen = 1.0f, ionFloat _clearBlue = 1.0f);


    //////////////////////////////////////////////////////////////////////////
    // Render

    void SetViewport(RenderCore& _renderCore, ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height);
    void SetScissor(RenderCore& _renderCore, ionS32 _x, ionS32 _y, ionS32 _width, ionS32 _height);
    void StartRenderPass(RenderCore& _renderCore);
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

    ECameraType m_type;

    ionFloat m_fov;
    ionFloat m_zNear;
    ionFloat m_zFar;

    ionFloat m_minDepth; 
    ionFloat m_maxDepth;

    ionFloat m_viewPortX;
    ionFloat m_viewPortY;
    ionFloat m_scissorX;
    ionFloat m_scissorY;

    ionFloat m_viewPortWidth;
    ionFloat m_viewPortHeight;
    ionFloat m_scissorWidth;
    ionFloat m_scissorHeight;

    ionFloat m_clearDepthValue;
    ionFloat m_clearRed;
    ionFloat m_clearGreen;
    ionFloat m_clearBlue;
    ionU8    m_clearStencilValue;
};

ION_NAMESPACE_END