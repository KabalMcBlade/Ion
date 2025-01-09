// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\App\CommandLineParser.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "CommandLineParser.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

OptionAllocator* Option::GetAllocator()
{
	static HeapArea<Settings::kOptionAllocatorSize> memoryArea;
	static OptionAllocator memoryAllocator(memoryArea, "OptionFreeListAllocator");

	return &memoryAllocator;
}

CommandLineParser::CommandLineParser()
{

}

CommandLineParser::~CommandLineParser()
{
    ionMap<ionString, Option*, OptionAllocator, Option::GetAllocator>::iterator it;
    for (it = m_options.begin(); it != m_options.end(); ++it)
    {
        ionDelete(it->second, Option::GetAllocator());
    }
    m_options.clear();
}

void CommandLineParser::Add(const ionString& _option, ionBool _mandatory /*= true*/)
{
    Option* opt = ionNew(Option, Option::GetAllocator());
    opt->SetOption(_option);
    opt->SetMandatory(_mandatory);
    
    m_options[_option] = opt;
}

ionBool CommandLineParser::Parse(ionS32 argc, const char * const argv[])
{
    // Parse the command line and set the value to the option
    // can fail if any errors occurred
    for (ionS32 i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-", 1) == 0) 
        {
			ionString name(argv[i]);
            if (m_options.count(name) == 0)
            {
                if (m_options[name]->IsMandatory())
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                m_options[name]->Set();
            }

            if (i + 1 >= argc) 
            {
                if (m_options[name]->NeedValue())
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (m_options[name]->NeedValue())
                {
                    ++i;
                    if (strncmp(argv[i], "-", 1) != 0)
                    {
                        m_options[name]->SetStringValue(argv[i]);
                    }
                    else if (m_options[name]->HasDefault())
                    {
                        continue;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

ionBool CommandLineParser::HasOption(const ionString& _option)
{
    return m_options.count(_option) > 0;
}

ionBool CommandLineParser::HasValue(const ionString& _option)
{
    auto search = m_options.find(_option);
    if (search != m_options.end())
    {
        return search->second->HasValue();
    }
    else
    {
        return false;
    }
}

ionBool CommandLineParser::IsSet(const ionString& _option)
{
    auto search = m_options.find(_option);
    if (search != m_options.end())
    {
        return search->second->IsSet();
    }
    else
    {
        return false;
    }
}

ION_NAMESPACE_END