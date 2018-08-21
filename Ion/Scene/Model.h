#pragma once

#include "../Core/CoreDefs.h"

#include "Entity.h"


ION_NAMESPACE_BEGIN

class Texture;
class ION_DLL Model : public Entity
{
public:
    explicit Model();
    explicit Model(const eosString & _name);
    virtual ~Model();

    ionBool Load(const eosString & _path, const eosString & _filename, ionBool _generateNormalWhenMissing = false, ionBool _generateTangentWhenMissing = false, ionBool _setBitangentSign = false);
    ionBool Init(Texture* _brdf, Texture* _irradiance, Texture* _prefiltered);

private:
    Model(const Model& _Orig) = delete;
    Model& operator = (const Model&) = delete;

private:

};

ION_NAMESPACE_END