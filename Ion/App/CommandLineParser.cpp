#include "CommandLineParser.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

CommandLineParser::CommandLineParser()
{

}

CommandLineParser::~CommandLineParser()
{
    eosMap(eosString, Option*)::iterator it;
    for (it = m_options.begin(); it != m_options.end(); ++it)
    {
        eosDelete(it->second);
    }
    m_options.clear();
}

void CommandLineParser::Add(const eosString& _option, ionBool _mandatory /*= true*/)
{
    Option* opt = eosNew(Option, ION_MEMORY_ALIGNMENT_SIZE);
    opt->SetOption(_option);
    opt->SetMandatory(_mandatory);
    
    m_options[_option] = opt;
}

ionBool CommandLineParser::Parse(ionS32 argc, const char * const argv[])
{
    // Parse the command line and set the value to the option
    // can fail if any errors occured
    for (ionS32 i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-", 1) == 0) 
        {
            eosString name(argv[i]);
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

ionBool CommandLineParser::HasOption(const eosString& _option)
{
    return m_options.count(_option) > 0;
}

ionBool CommandLineParser::HasValue(const eosString& _option)
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

ionBool CommandLineParser::IsSet(const eosString& _option)
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