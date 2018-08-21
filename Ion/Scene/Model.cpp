#include "Model.h"

ION_NAMESPACE_BEGIN

Model::Model() : Entity()
{
}

Model::Model(const eosString & _name) : Entity(_name)
{
}

Model::~Model()
{

}

ionBool Model::Load(const eosString & _path, const eosString & _filename, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    eosString modelPath = _path;
    modelPath += _filename;
    //ionRenderManager().LoadModelFromFile(modelPath, );
    return false;
}

ION_NAMESPACE_END