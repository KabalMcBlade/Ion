// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\App\CommandLineParser.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"


#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


using OptionAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class ION_DLL Option
{
public:
	static OptionAllocator* GetAllocator();

public:
    ION_INLINE Option();
    ION_INLINE virtual ~Option();

    ION_INLINE virtual ionBool HasValue() const { return false; }
    ION_INLINE virtual ionBool NeedValue() const { return false; }
    ION_INLINE virtual ionBool HasDefault() const { return false; }

    ION_INLINE void SetOption(const ionString& _option);       // -width, -height, etc etc
    ION_INLINE ionBool IsSet() const;
    ION_INLINE ionBool IsMandatory() const;

    ION_INLINE void Set();
    ION_INLINE void SetMandatory(ionBool _isMandatory);

    ION_INLINE const ionString& GetOption() const { return m_option; }

    ION_INLINE void SetStringValue(const ionString& _value);

protected:
	ionString m_option;
	ionString m_stringValue;
    ionBool m_isMandatory;
    ionBool m_isSet;
};


ION_INLINE Option::Option() : m_isMandatory(false), m_isSet(false)
{

}

ION_INLINE Option::~Option()
{

}

ION_INLINE void Option::SetOption(const ionString& _option)
{
    m_option = _option;
}

ION_INLINE ionBool Option::IsSet() const
{
    return m_isSet;
}

ION_INLINE ionBool Option::IsMandatory() const
{
    return m_isMandatory;
}

ION_INLINE void Option::SetMandatory(ionBool _isMandatory)
{
    m_isMandatory = _isMandatory;
}

ION_INLINE void Option::SetStringValue(const ionString& _value)
{
    m_stringValue =  _value;
    std::transform(m_stringValue.begin(), m_stringValue.end(), m_stringValue.begin(), ::tolower);
}

ION_INLINE void Option::Set()
{
    m_isSet = true;
}

//////////////////////////////////////////////////////////////////////////

template <class T>
class OptionValue : public Option
{
public:
    OptionValue();
    virtual ~OptionValue();

    virtual ionBool NeedValue() const override;
    virtual ionBool HasValue() const override;
    virtual ionBool HasDefault() const override;

    void SetDefault(T _value);
    const T& GetValue() const;
    void Parse();

private:
    T m_value;
    T m_default;
    ionBool m_hasDefault;
};

template <class T>
OptionValue<T>::OptionValue() : Option(), m_hasDefault(false)
{

}

template <class T>
OptionValue<T>::~OptionValue()
{

}

template <class T>
void OptionValue<T>::Parse()
{
    T var;
    ionIStringStream iss;
    iss.str(m_stringValue);
    iss >> var;
    m_value = var;
}

template <class T>
ionBool OptionValue<T>::NeedValue() const
{
    return true;
}

template <class T>
ionBool OptionValue<T>::HasValue() const
{
    return !m_stringValue.empty();
}

template <class T>
ionBool OptionValue<T>::HasDefault() const
{
    return m_hasDefault;
}

template <class T>
void OptionValue<T>::SetDefault(T _value)
{
    m_default = _value;
    m_hasDefault = true;
}

template <class T>
const T& OptionValue<T>::GetValue() const
{
    return HasValue() ? m_value : m_default;
}

//////////////////////////////////////////////////////////////////////////

template <>
class OptionValue<ionString> : public Option
{
public:
    ION_INLINE OptionValue();
    ION_INLINE virtual ~OptionValue();

    ION_INLINE virtual ionBool NeedValue() const override;
    ION_INLINE virtual ionBool HasValue() const override;
    ION_INLINE virtual ionBool HasDefault() const override;

    ION_INLINE void SetDefault(ionString _value);
    ION_INLINE const ionString& GetValue() const;
    ION_INLINE void Parse();

private:
	ionString m_value;
	ionString m_default;
    ionBool m_hasDefault;
};

ION_INLINE OptionValue<ionString>::OptionValue() : Option(), m_hasDefault(false)
{

}

ION_INLINE OptionValue<ionString>::~OptionValue()
{

}

ION_INLINE void OptionValue<ionString>::Parse()
{
    m_value = m_stringValue;
}

ION_INLINE ionBool OptionValue<ionString>::NeedValue() const
{
    return true;
}

ION_INLINE ionBool OptionValue<ionString>::HasValue() const
{
    return !m_stringValue.empty();
}

ION_INLINE ionBool OptionValue<ionString>::HasDefault() const
{
    return m_hasDefault;
}

ION_INLINE void OptionValue<ionString>::SetDefault(ionString _value)
{
    m_default = _value;
    m_hasDefault = true;
}

ION_INLINE const ionString& OptionValue<ionString>::GetValue() const
{
    return HasValue() ? m_value : m_default;
}


//////////////////////////////////////////////////////////////////////////

class ION_DLL CommandLineParser final
{
public:
    CommandLineParser();
    ~CommandLineParser();

    void Add(const ionString& _option, ionBool _mandatory = true);

    template <class T>
    void AddWithValue(const ionString& _option, ionBool _mandatory = true);

    template <class T>
    void AddWithValueAndDefault(const ionString& _option, ionBool _mandatory = true, const T _default = T());


    ionBool Parse(ionS32 argc, const char * const argv[]);

    ionBool HasOption(const ionString& _option);

    ionBool HasValue(const ionString& _option);
    ionBool IsSet(const ionString& _option);

    template <class T>
    T GetValue(const ionString& _option);

private:
    ionMap<ionString, Option*, OptionAllocator, Option::GetAllocator> m_options;
};

template <class T>
void CommandLineParser::AddWithValue(const ionString& _option, ionBool _mandatory /*= true*/)
{
    OptionValue<T>* opt = ionNew(OptionValue<T>, Option::GetAllocator());
    opt->SetOption(_option);
    opt->SetMandatory(_mandatory);

    m_options[_option] = opt;
}

template <class T>
void CommandLineParser::AddWithValueAndDefault(const ionString& _option, ionBool _mandatory /*= true*/, const T _default /*= T()*/)
{
    OptionValue<T>* opt = ionNew(OptionValue<T>, Option::GetAllocator());
    opt->SetOption(_option);
    opt->SetMandatory(_mandatory);
    opt->SetDefault(_default);
    opt->SetStringValue(std::to_string(_default).c_str());  // redundant but easy to do

    m_options[_option] = opt;
}

template <class T>
T CommandLineParser::GetValue(const ionString& _option)
{
    auto search = m_options.find(_option);
    if (search != m_options.end())
    {
        OptionValue<T>* opt = dynamic_cast<OptionValue<T>*>(search->second);
        if (opt->HasValue())
        {
            opt->Parse();
            return opt->GetValue();
        }
        else
        {
            return T();
        }
    }
    else
    {
        return T();
    }
}

ION_NAMESPACE_END