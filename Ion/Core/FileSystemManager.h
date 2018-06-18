#pragma once

#include "CoreDefs.h"
#include "StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL FileSystemManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static FileSystemManager& Instance();

    ionBool Init(const eosString& _mainPath, const eosString& _shadersPath, const eosString& _texturesPath, const eosString& _modelsPath);
    void    Shutdown();

    FileSystemManager();
    ~FileSystemManager();

    const eosString& GetMainPath() const { return m_mainPath; }
    const eosString& GetShadersPath() const { return m_shadersPath; }
    const eosString& GetTexturesPath() const { return m_texturesPath; }
    const eosString& GetModelsPath() const { return m_modelsPath; }

private:
    FileSystemManager(const FileSystemManager& _Orig) = delete;
    FileSystemManager& operator = (const FileSystemManager&) = delete;

    ionBool GetFullPath(const eosString& partialPath, eosString& fullPath);

private:
    eosString m_mainPath;
    eosString m_shadersPath;
    eosString m_texturesPath;
    eosString m_modelsPath;

private:
    static FileSystemManager *s_instance;
};

ION_NAMESPACE_END


#define ionFileSystemManager() ion::FileSystemManager::Instance()