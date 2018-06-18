#include "FileSystemManager.h"


EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN


FileSystemManager *FileSystemManager::s_instance = nullptr;


FileSystemManager::FileSystemManager()
{
}

FileSystemManager::~FileSystemManager()
{

}

void FileSystemManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(FileSystemManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void FileSystemManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

FileSystemManager& FileSystemManager::Instance()
{
    return *s_instance;
}

ionBool FileSystemManager::Init(const eosString& _mainPath, const eosString& _shadersPath, const eosString& _texturesPath, const eosString& _modelsPath)
{
    GetFullPath("./", m_mainPath);

    const eosString from = "\\";
    const eosString to = "/";

    ionSize start_pos = 0;
    while ((start_pos = m_mainPath.find(from, start_pos)) != std::string::npos)
    {
        m_mainPath.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    m_mainPath.append(_mainPath);
    m_mainPath.append("/");

    m_shadersPath = m_mainPath;
    m_shadersPath.append(_shadersPath);
    m_shadersPath.append("/");

    m_texturesPath = m_mainPath;
    m_texturesPath.append(_texturesPath);
    m_texturesPath.append("/");

    m_modelsPath = m_mainPath;
    m_modelsPath.append(_modelsPath);
    m_modelsPath.append("/");

    return true;
}

void FileSystemManager::Shutdown()
{

}

ionBool FileSystemManager::GetFullPath(const eosString& partialPath, eosString& fullPath)
{
    char full[_MAX_PATH];
    if (_fullpath(full, partialPath.c_str(), _MAX_PATH) != NULL)
    {
        fullPath = full;
        return true;
    }
    return false;
}


ION_NAMESPACE_END