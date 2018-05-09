#include "RenderManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Entity.h"

#include "../Utilities/LoaderGLTF.h"

#include "VertexCacheManager.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


RenderManager *RenderManager::s_instance = nullptr;


RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{

}

ionBool RenderManager::Init(HINSTANCE _instance, HWND _handle, ionU32 _width, ionU32 _height, ionBool _fullScreen, ionBool _enableValidationLayer, const eosString& _shaderFolderPath, ionSize _vkDeviceLocalSize, ionSize _vkHostVisibleSize, ionSize _vkStagingBufferSize)
{
    return m_renderCore.Init(_instance, _handle, _width, _height, _fullScreen, _enableValidationLayer, _shaderFolderPath, _vkDeviceLocalSize, _vkHostVisibleSize, _vkStagingBufferSize);
}

void RenderManager::Shutdown()
{
    m_renderCore.Shutdown();
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

void RenderManager::AddCamera(BaseCamera& _camera)
{
    m_cameraList.push_back(&_camera);
}

void RenderManager::AddEntity(Entity& _entity)
{
    m_entityList.push_back(&_entity);
}

void RenderManager::PreRender()
{
    for (auto const& entityRef : m_entityList)
    {
        const Entity& entity = (*entityRef);

        const eosVector(Mesh)& meshList = entity.GetMeshList();
        for (auto const& meshRef : meshList)
        {
            const Mesh& mesh = (meshRef);

            const eosVector(Primitive)& primitiveList = mesh.GetPrimitives();
            for (auto const& primRef : primitiveList)
            {
                const Primitive& primitive = (primRef);

                const VertexCacheHandler vertexHandler = ionVertexCacheManager().AllocVertex(primitive.m_vertexes.data(), primitive.m_vertexes.size());
                const VertexCacheHandler indexHandler = ionVertexCacheManager().AllocIndex(primitive.m_indexes.data(), primitive.m_indexes.size());

                m_vertexHandlers.push_back(vertexHandler);
                m_indexHandlers.push_back(indexHandler);
            }
        }
    }
}

void RenderManager::Render()
{
    m_renderCore.StartFrame();



    m_renderCore.EndFrame();
}

ION_NAMESPACE_END