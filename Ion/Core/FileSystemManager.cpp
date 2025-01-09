// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Core\FileSystemManager.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "FileSystemManager.h"


EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN

FileSystemManager::FileSystemManager()
{
}

FileSystemManager::~FileSystemManager()
{

}

FileSystemManager& FileSystemManager::Instance()
{
    static FileSystemManager instance;
    return instance;
}

ionBool FileSystemManager::Init(const ionString& _mainPath, const ionString& _shadersPath, const ionString& _texturesPath, const ionString& _modelsPath)
{
    GetFullPath("./", m_mainPath);

    const ionString from = "\\";
    const ionString to = "/";

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

ionBool FileSystemManager::GetFullPath(const ionString& partialPath, ionString& fullPath)
{
	char tmp[256];
    if (_fullpath(tmp, partialPath.c_str(), _MAX_PATH) != NULL)
    {
		fullPath = tmp;
        return true;
    }
    return false;
}


ION_NAMESPACE_END