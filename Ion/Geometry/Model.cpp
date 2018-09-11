#include "Model.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////////
BaseModel::BaseModel()
{
    m_layout = EVertexLayout_Empty;
}

BaseModel::~BaseModel()
{
    m_indices.clear();
    m_meshes.clear();
}

void BaseModel::PushBackIndex(const Index& _index)
{
    m_indices.push_back(_index);
}

const void* BaseModel::GetIndexData() const
{
    return m_indices.data();
}

EVertexLayout BaseModel::GetLayout() const
{
    return m_layout;
}

void BaseModel::PushBackMeshInfo(ionU32 _indexStart, ionU32 _indexCount, Material* _material)
{
    MeshPrototype mesh;
    mesh.Set(_indexStart, _indexCount, _material);
    m_meshes.push_back(mesh);
}

//////////////////////////////////////////////////////////////////////////
ModelPlain::ModelPlain() : BaseModel()
{
    m_layout = EVertexLayout_Pos;
}

ModelPlain::~ModelPlain()
{
    m_vertices.clear();
}

void ModelPlain::PushBackVertex(const VertexPlain& _vertex)
{
    m_vertices.push_back(_vertex);
}

const void* ModelPlain::GetVertexData() const
{
    return m_vertices.data();
}

//////////////////////////////////////////////////////////////////////////
ModelColored::ModelColored() : BaseModel()
{
    m_layout = EVertexLayout_Pos_Color;
}

ModelColored::~ModelColored()
{
    m_vertices.clear();
}

void ModelColored::PushBackVertex(const VertexColored& _vertex)
{
    m_vertices.push_back(_vertex);
}

const void* ModelColored::GetVertexData() const
{
    return m_vertices.data();
}

//////////////////////////////////////////////////////////////////////////
ModelUV::ModelUV() : BaseModel()
{
    m_layout = EVertexLayout_Pos_UV;
}

ModelUV::~ModelUV()
{
    m_vertices.clear();
}

void ModelUV::PushBackVertex(const VertexUV& _vertex)
{
    m_vertices.push_back(_vertex);
}

const void* ModelUV::GetVertexData() const
{
    return m_vertices.data();
}

//////////////////////////////////////////////////////////////////////////
ModelSimple::ModelSimple() : BaseModel()
{
    m_layout = EVertexLayout_Pos_UV_Normal;
}

ModelSimple::~ModelSimple()
{
    m_vertices.clear();
}

void ModelSimple::PushBackVertex(const VertexSimple& _vertex)
{
    m_vertices.push_back(_vertex);
}

const void* ModelSimple::GetVertexData() const
{
    return m_vertices.data();
}

//////////////////////////////////////////////////////////////////////////
Model::Model() : BaseModel()
{
    m_layout = EVertexLayout_Full;
}

Model::~Model()
{
    m_vertices.clear();
}

void Model::PushBackVertex(const Vertex& _vertex)
{
    m_vertices.push_back(_vertex);
}

const void* Model::GetVertexData() const
{
    return m_vertices.data();
}



ION_NAMESPACE_END


