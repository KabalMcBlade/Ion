// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Core\MemoryWrapper.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


// for simplicity, every vector,set,etc is aligned to 16

template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void)> using ionVector = Vector<T, Allocator, _AllocatorCallback, 16>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void)> using ionList = List<T, Allocator, _AllocatorCallback, 16>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void)> using ionStack = Stack<T, Allocator, _AllocatorCallback, 16>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void)> using ionDeque = Deque<T, Allocator, _AllocatorCallback, 16>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void)> using ionQueue = Queue<T, Allocator, _AllocatorCallback, 16>;

template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>> using ionMap = Map<K, V, Allocator, _AllocatorCallback, Compare, 16>;
template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>> using ionUnorderedMap = UnorderedMap<K, V, Allocator, _AllocatorCallback, Compare, 16>;

template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<T>> using ionSet = Set<T, Allocator, _AllocatorCallback, Compare, 16>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Hasher = std::hash<T>, typename KeyEquality = std::equal_to<T>> using ionUnorderedSet = UnorderedSet<T, Allocator, _AllocatorCallback, Hasher, KeyEquality, 16>;


// FOR STRING AND STREAM I USE ONE ALLOCATOR ONLY! THIS IS CONVENIENT!

using CommonAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

extern ION_DLL CommonAllocator* GetCommonAllocator();

using ionString = String<CommonAllocator, GetCommonAllocator>;
using ionWString = WString<CommonAllocator, GetCommonAllocator>;

using ionOStream = OStream<CommonAllocator, GetCommonAllocator>;
using ionStringStream = StringStream<CommonAllocator, GetCommonAllocator>;
using ionIStringStream = IStringStream<CommonAllocator, GetCommonAllocator>;

using ionWOStream = WOStream<CommonAllocator, GetCommonAllocator>;
using ionWStringStream = WStringStream<CommonAllocator, GetCommonAllocator>;
using ionWIStringStream = WIStringStream<CommonAllocator, GetCommonAllocator>;




ION_NAMESPACE_END


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


#define ionNewAligned(Type, Allocator, Alignment, ...)  eosNewAligned(Type, Allocator, Alignment, __VA_ARGS__)
#define ionNew(Type, Allocator, ...)					ionNewAligned(Type, Allocator, ION_MEMORY_ALIGNMENT_SIZE, __VA_ARGS__)
#define ionDelete(Object, Allocator)					eosDelete(Object, Allocator)

#define ionNewAlignedRaw(Size, Allocator, Alignment)	eosNewAlignedRaw(Size, Allocator, Alignment)
#define ionNewRaw(Size, Allocator)                      ionNewAlignedRaw(Size, Allocator, ION_MEMORY_ALIGNMENT_SIZE)
#define ionDeleteRaw(Object, Allocator)					eosDeleteRaw(Object, Allocator)

#define ionNewDynamicArray(Type, Count, Allocator)		eosNewDynamicArray(Type, Count, Allocator)
#define ionNewArray(Type, Allocator)					eosNewArray(Type, Allocator)
#define ionDeleteArray(ObjectArray, Allocator)			eosDeleteArray((ObjectArray), (Allocator))



namespace std
{
	template<>
    struct hash<ion::ionString>
    {
        typedef ion::ionString argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& _value) const noexcept
        {
            result_type const h(std::hash<std::string>{}(_value.c_str()));
            return h;
        }
    };
}
