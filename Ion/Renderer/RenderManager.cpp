#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"
#include "DrawSurface.h"

#include "../Geometry/Mesh.h"


//#define SHADOW_MAP_SIZE					1024

#define ION_CACHE_LINE_SIZE	    128
#define ION_MAX_FRAME_MEMORY    67305472    //64 * 1024 * 1024



EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager() : m_nodeCount(0), m_frameData(nullptr), m_smpFrame(0)
{
    memset(&m_smpFrameData, 0, sizeof(m_smpFrameData));
}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    if (m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _shaderFolderPath, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize))
    {
        InitFrameData();
        //SwapCommandBuffers(nullptr);

        return true;
    }

    return false;
}

void RenderManager::Shutdown()
{
    ShutdownFrameData();
    //UnbindBufferObjects();
    m_renderCore.Shutdown();

    ShutdownFrameData();
}

void RenderManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(RenderManager, EOS_MEMORY_ALIGNMENT_SIZE);
    }
}

void RenderManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

RenderManager& RenderManager::Instance()
{
    return *s_instance;
}

ionBool RenderManager::LoadModelFromFile(const eosString& _fileName, Entity& _entity)
{
    LoaderGLTF loader;
    return loader.Load(_fileName, m_renderCore.GetDevice(), _entity);
}

void RenderManager::AddScene(NodeHandle& _root)
{
    // simplify version for now, just to have something to render soon
    const eosVector(NodeHandle)& children = _root->GetChildren();

    eosVector(NodeHandle)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;

    for (; it != end; ++it)
    {
        NodeHandle nh = (*it);

        switch(nh->GetNodeType())
        {
        case ENodeType_Camera:
            if (!m_mainCamera.IsValid())
            {
                m_mainCamera.SetFromOther<Node>(nh);
            }
            break;
        case ENodeType_Entity:
            EntityHandle handle;
            handle.SetFromOther<Node>(nh);
            m_sceneBoundingBox.Expande(handle->GetTransformedBoundingBox());
            m_entityNodes.push_back(handle);
            break;
        }
    }
}

void RenderManager::AddScene(Node& _root)
{
    SmartPointer<Node> rootHandle(&_root);
    AddScene(rootHandle);
}

void RenderManager::Resize()
{
	m_renderCore.Recreate();
}

void* RenderManager::FrameAlloc(ionS32 _bytes)
{
    _bytes = (_bytes + ION_FRAME_ALLOC_ALIGNMENT - 1) & ~(ION_FRAME_ALLOC_ALIGNMENT - 1);

    // thread safe add
    m_frameData->m_frameMemoryAllocated.fetch_add(_bytes);
    ionSize end = m_frameData->m_frameMemoryAllocated.load();
    ionAssertReturnValue(end <= ION_MAX_FRAME_MEMORY, "FrameAlloc ran out of memory", nullptr);

    ionU8* ptr = m_frameData->m_frameMemory + end - _bytes;

    // cache line clear the memory
    for (ionS32 offset = 0; offset < _bytes; offset += ION_CACHE_LINE_SIZE)
    {
        ionU8* bytePtr = (ionU8*)((((UINT_PTR)(ptr)) + (offset)) & ~(ION_CACHE_LINE_SIZE - 1));
        memset(bytePtr, 0, ION_CACHE_LINE_SIZE);
    }

    return ptr;
}


void RenderManager::ToggleSmpFrame()
{
    m_smpFrame++;
    m_frameData = &m_smpFrameData[m_smpFrame % ION_FRAME_DATA_COUNT];

    const ionU32 bytesNeededForAlignment = ION_FRAME_ALLOC_ALIGNMENT - ((ionU32)m_frameData->m_frameMemory & (ION_FRAME_ALLOC_ALIGNMENT - 1));
    m_frameData->m_frameMemoryAllocated.fetch_add(bytesNeededForAlignment);
    m_frameData->m_frameMemoryUsed.fetch_add(0);

    // clear the command chain
    m_frameData->m_renderCommandIndex = 0;
    m_frameData->m_renderCommands.clear();
}

void RenderManager::InitFrameData()
{
    ShutdownFrameData();

    for (ionS32 i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        m_smpFrameData[i].m_frameMemory = (ionU8*)eosNewRaw(ION_MAX_FRAME_MEMORY, EOS_MEMORY_ALIGNMENT_SIZE);
    }

    // must be set before ToggleSmpFrame()
    m_frameData = &m_smpFrameData[0];

    ToggleSmpFrame();
}

void RenderManager::ShutdownFrameData()
{
    m_frameData = nullptr;
    for (int i = 0; i < ION_FRAME_DATA_COUNT; ++i)
    {
        eosDeleteRaw(m_smpFrameData[i].m_frameMemory);
        m_smpFrameData[i].m_frameMemory = nullptr;
    }
}

void RenderManager::AddDrawViewCmd(/*viewDef_t *parms*/)
{
    RenderCommand& cmd = m_frameData->m_renderCommands[m_frameData->m_renderCommandIndex++];
    cmd.m_operation = ERenderOperation_Draw_View;
    //cmd.m_viewDef = parms;
}


void RenderManager::RenderCommandBuffers()
{
    // Use the previous SMP frame data as the current is being written to.
    FrameData & frameData = m_smpFrameData[(m_smpFrame - 1) % ION_FRAME_DATA_COUNT];

    // if there isn't a draw view command, do nothing to avoid swapping a bad frame
    if (frameData.m_renderCommandIndex == 0) 
    {
        return;
    }
    if (frameData.m_renderCommands[0].m_operation == ERenderOperation_None)
    {
        return;
    }

    m_renderCore.Execute(frameData.m_renderCommandIndex, frameData.m_renderCommands);
}

void RenderManager::Prepare()
{    
    /*
    const ionSize nodeCount = m_entityNodes.size();
    for (ionSize i = 0; i < nodeCount; ++i)
    {
        VertexCacheHandler vertexCache = ionVertexCacheManager().AllocVertex(m_entityNodes[i]->GetVertexBuffer(0, 0), m_entityNodes[i]->GetVertexBufferSize(0, 0));
        VertexCacheHandler indexCache = ionVertexCacheManager().AllocIndex(m_entityNodes[i]->GetIndexBuffer(0, 0), m_entityNodes[i]->GetIndexBufferSize(0, 0));

        m_vertexCache.push_back(vertexCache);
        m_indexCache.push_back(indexCache);
    }
    */

    //
    // Update entities
    m_nodeCount = m_entityNodes.size();

    // temp: need a proper way to reserve all the amount of surfaces
    if (m_drawSurfaces.capacity() == 0)
    {
        m_drawSurfaces.resize(m_nodeCount);
    }

    // because I want to alternate the buffer, before the first frame I'm preparing the 0 index, which will be presented in the first frame.
    //
    m_mainCamera->Update();

    //
    const Matrix& projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix& view = m_mainCamera->GetView();

    UpdateDrawSurface(projection, view, m_nodeCount);

    //ionVertexCacheManager().BeginFrame();
}

void RenderManager::UpdateDrawSurface(const Matrix& _projection, const Matrix& _view, ionSize _nodeCount)
{
    for (ionSize i = 0; i < _nodeCount; ++i)
    {
        //
        // here we need to update the entity position

        m_entityNodes[i]->GetTransformHandle()->UpdateTransform();
        //m_entityNodes[i]->GetTransformHandle()->UpdateTransformInverse();

        const Matrix& model = m_entityNodes[i]->GetTransformHandle()->GetMatrix();
        //const Matrix& model = m_entityNodes[i]->GetTransformHandle()->GetMatrixInverse();

        // not aligned... just to test
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[0], model[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[4], model[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[8], model[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_modelMatrix[12], model[3]);

        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[0], _view[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[4], _view[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[8], _view[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_viewMatrix[12], _view[3]);

        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[0], _projection[0]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[4], _projection[1]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[8], _projection[2]);
        _mm_storeu_ps(&m_drawSurfaces[i].m_projectionMatrix[12], _projection[3]);

        m_drawSurfaces[i].m_indexCount = m_entityNodes[i]->GetIndexBufferSize(0, 0);
        m_drawSurfaces[i].m_vertexCache = ionVertexCacheManager().AllocVertex(m_entityNodes[i]->GetVertexBuffer(0, 0), m_entityNodes[i]->GetVertexBufferSize(0, 0));
        m_drawSurfaces[i].m_indexCache = ionVertexCacheManager().AllocIndex(m_entityNodes[i]->GetIndexBuffer(0, 0), m_entityNodes[i]->GetIndexBufferSize(0, 0));
        m_drawSurfaces[i].m_material = m_entityNodes[i]->GetMaterial(0, 0);
    }

}

void RenderManager::CoreLoop()
{
    Update();
    DrawFrame();
}

void RenderManager::Update()
{
    //
    m_mainCamera->Update();

    //
    const Matrix& projection = m_mainCamera->GetPerspectiveProjection();
    const Matrix& view = m_mainCamera->GetView();

    UpdateDrawSurface(projection, view, m_nodeCount);
}

void RenderManager::DrawFrame()
{
    const ionU32 width = m_renderCore.GetWidth();
    const ionU32 height = m_renderCore.GetHeight();
    const ionSize drawSurfacesCount = m_drawSurfaces.size();

    m_renderCore.BlockingSwapBuffers();
    ionVertexCacheManager().BeginFrame();
    if (m_renderCore.StartFrame())
    {
        m_renderCore.SetViewport(0, 0, width, height);
        m_renderCore.SetScissor(0, 0, width, height);
        m_renderCore.SetState(ECullingMode_Front);
        m_renderCore.SetClear(true, true, true, ION_STENCIL_SHADOW_TEST_VALUE, 1.0f, 0.0f, 0.0f, 0.0f);
        
        /*
        for (ionSize i = 0; i < drawSurfacesCount; ++i)
        {
            m_renderCore.Draw(m_drawSurfaces[i]);
        }
        */
        m_renderCore.EndFrame();
    }
}


ION_NAMESPACE_END