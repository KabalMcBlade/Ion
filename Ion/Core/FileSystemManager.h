#pragma once

#include "CoreDefs.h"
#include "StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Core/MemoryWrapper.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL FileSystemManager final
{
public:
    static FileSystemManager& Instance();

    ionBool Init(const ionString& _mainPath, const ionString& _shadersPath, const ionString& _texturesPath, const ionString& _modelsPath);
    void    Shutdown();

    FileSystemManager();
    ~FileSystemManager();

    const ionString& GetMainPath() const { return m_mainPath; }
    const ionString& GetShadersPath() const { return m_shadersPath; }
    const ionString& GetTexturesPath() const { return m_texturesPath; }
    const ionString& GetModelsPath() const { return m_modelsPath; }

private:
    FileSystemManager(const FileSystemManager& _Orig) = delete;
    FileSystemManager& operator = (const FileSystemManager&) = delete;

    ionBool GetFullPath(const ionString& partialPath, ionString& fullPath);

private:
    ionString m_mainPath;
    ionString m_shadersPath;
    ionString m_texturesPath;
    ionString m_modelsPath;
};

ION_NAMESPACE_END


#define ionFileSystemManager() ion::FileSystemManager::Instance()