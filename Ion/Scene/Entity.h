#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Node.h"

#include "Mesh.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL Entity : public Node
{
public:
    explicit Entity();
    explicit Entity(const eosString & _name);
    virtual ~Entity();

    ionBool LoadFromGLTF(const eosString & _fileName);  // just version 2

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    eosVector(Mesh)    m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END