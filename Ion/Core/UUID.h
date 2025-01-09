// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Core\UUID.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Core/CoreDefs.h"
#include "../Core/StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Core/MemoryWrapper.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class ION_DLL UUID
{
public:
    static const UUID Empty;
    static volatile ionU32 s_counter;

    UUID() : m_highBites(0), m_lowBites(0) { }
    UUID(const UUID& _other) : m_highBites(_other.m_highBites), m_lowBites(_other.m_lowBites) { }

    ION_INLINE bool IsValid() const { return m_highBites != 0; }

    void GenerateUUID();

    const UUID& operator= (const UUID& rhs) { m_highBites = rhs.m_highBites; m_lowBites = rhs.m_lowBites; return *this; }
    bool operator== (const UUID& rhs) const { return m_highBites == rhs.m_highBites && m_lowBites == rhs.m_lowBites; }
    bool operator!= (const UUID& rhs) const { return m_highBites != rhs.m_highBites || m_lowBites != rhs.m_lowBites; }

    const ionString& ToString() const { return m_uuidString; }

    ionU32 GetIndex() const { return m_index; }

private:
    ionString m_uuidString;

    union
    {
        struct
        {
            ionU32  m_timeStamp;
            ionU32  m_macAddressHigh;
            ionU32  m_randomNumber;
            ionU32  m_index;
        };
        struct
        {
            ionU64  m_highBites;
            ionU64  m_lowBites;
        };
    };

    static ionU32 g_counter;
};

ION_NAMESPACE_END