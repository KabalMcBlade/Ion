#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "TextureCommon.h"
#include "TextureOptions.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class Texture final
{
public:
    Texture(const eosString& _name);
    ~Texture();

    const eosString& GetName() const { return m_name; }
    const TextureOptions& GetOptions() const { return m_options; }

    void SetOptions(const TextureOptions& _options);

    void Create();
    void Destroy();

private:
    eosString       m_name;
    TextureOptions	m_options;
};


ION_NAMESPACE_END