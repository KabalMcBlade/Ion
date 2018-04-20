#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Entity;

static ionBool LoadFromGLTF(const eosString & _fileName, Entity& _entity);  // just version 2


ION_NAMESPACE_END