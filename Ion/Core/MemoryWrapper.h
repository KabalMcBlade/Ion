#pragma once

#include "CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "MemoryManager.h"

EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////
template<typename T>
class ionVector final
{
public:
    using value_type = T;
    using allocator_type = HeapAllocatorVector;
    using pointer = typename eosVector<T, HeapAllocatorVector>::pointer;
    using const_pointer = typename eosVector<T, HeapAllocatorVector>::const_pointer;
    using reference = T & ;
    using const_reference = const T&;
    using size_type = typename eosVector<T, HeapAllocatorVector>::size_type;
    using difference_type = typename eosVector<T, HeapAllocatorVector>::difference_type;
    using iterator = typename eosVector<T, HeapAllocatorVector>::iterator;
    using const_iterator = typename eosVector<T, HeapAllocatorVector>::const_iterator;
    using reverse_iterator = _STD reverse_iterator<iterator>;
    using const_reverse_iterator = _STD reverse_iterator<const_iterator>;

public:
    ionVector() : m_vector(MemoryManager::Instance().GetVectorAllocator()) {}
    ionVector(std::initializer_list<T> _list) : m_vector(MemoryManager::Instance().GetVectorAllocator())
    {
        m_vector.insert(m_vector.end(), _list.begin(), _list.end());
    }

    value_type& operator[](size_type idx) { return m_vector[idx]; }
    const value_type& operator[](size_type idx) const { return m_vector[idx]; }

public:
    eosVector<T, HeapAllocatorVector>& operator*() { return *m_vector; }
    eosVector<T, HeapAllocatorVector>* operator->() { return &m_vector; }

    const eosVector<T, HeapAllocatorVector>& operator*() const { return *m_vector; }
    const eosVector<T, HeapAllocatorVector>* operator->() const { return &m_vector; }

private:
    eosVector<T, HeapAllocatorVector> m_vector;
};


////////////////////////////////////////////////////////////////
template<typename K, typename V, typename Compare = std::less<K>>
class ionMap final
{
public:
    using _Mybase = eosMap<K, V, HeapAllocatorMap, Compare>;

    using key_type = K;
    using mapped_type = V;
    using key_compare = Compare;

    using value_compare = typename _Mybase::value_compare;
    using value_type = typename _Mybase::value_type;
    using allocator_type = typename _Mybase::allocator_type;
    using size_type = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer = typename _Mybase::pointer;
    using const_pointer = typename _Mybase::const_pointer;
    using reference = value_type & ;
    using const_reference = const value_type&;
    using iterator = typename _Mybase::iterator;
    using const_iterator = typename _Mybase::const_iterator;
    using reverse_iterator = typename _Mybase::reverse_iterator;
    using const_reverse_iterator = typename _Mybase::const_reverse_iterator;

    using _Alnode = typename _Mybase::_Alnode;
    using _Alnode_traits = typename _Mybase::_Alnode_traits;
    using _Pairib = typename _Mybase::_Pairib;

public:
    ionMap() : m_map(MemoryManager::Instance().GetMapAllocator()) {}

    mapped_type& operator[](key_type key) { return m_map[key]; }
    const mapped_type& operator[](key_type key) const { return m_map[key]; }

public:
    eosMap<K, V, HeapAllocatorMap, Compare>& operator*() { return *m_map; }
    eosMap<K, V, HeapAllocatorMap, Compare>* operator->() { return &m_map; }

    const eosMap<K, V, HeapAllocatorMap, Compare>& operator*() const { return *m_map; }
    const eosMap<K, V, HeapAllocatorMap, Compare>* operator->() const { return &m_map; }

private:
	_Mybase m_map;
};

////////////////////////////////////////////////////////////////
class ionString final
{
public:
    using value_type = char;
    using allocator_type = HeapAllocatorString;
    using pointer = typename eosString<HeapAllocatorString>::pointer;
    using const_pointer = typename eosString<HeapAllocatorString>::const_pointer;
    using reference = char & ;
    using const_reference = const char&;
    using size_type = typename eosString<HeapAllocatorString>::size_type;
    using difference_type = typename eosString<HeapAllocatorString>::difference_type;
    using iterator = typename eosString<HeapAllocatorString>::iterator;
    using const_iterator = typename eosString<HeapAllocatorString>::const_iterator;
    using reverse_iterator = _STD reverse_iterator<iterator>;
    using const_reverse_iterator = _STD reverse_iterator<const_iterator>;

public:
    ionString() : m_string(MemoryManager::Instance().GetStringAllocator()) {}
	ionString(const char* _v) : m_string(_v, MemoryManager::Instance().GetStringAllocator()) {}
    ionString(const ionString& _other) : m_string(_other.m_string) {}
    ionString(const eosString<HeapAllocatorString>& _other) : m_string(_other) {}

public:
    const_iterator cbegin()
    {
        return m_string.cbegin();
    }

    const_iterator cend()
    {
        return m_string.cend();
    }

    iterator begin()
    {
        return m_string.begin();
    }

    iterator end()
    {
        return m_string.end();
    }

    bool empty() const
    {
        return m_string.empty();
    }

    size_type find(const ionString& _Right, const size_type _Off = 0) const noexcept
    {	
        return m_string.find(_Right.c_str(), _Off);
    }

    ionString& replace(const size_type _Off, const size_type _N0, const ionString& _Right)
    {	
        m_string = m_string.replace(_Off, _N0, _Right.m_string);
		return *this;
    }

    ionString& append(const ionString& _Right)
    {	
        m_string = m_string.append(_Right.m_string);
		return *this;
    }

    size_type length() const noexcept
    {
        return m_string.length();
    }

    const char* c_str() const 
    {
        return m_string.c_str();
    }

	char* str()
	{
		return &m_string[0];
	}

    bool equal(const ionString& _right) const
    {	
        return (strcmp(m_string.c_str(), _right.m_string.c_str()) == 0);
    }

    size_type rfind(char _Char, const size_type _Off = std::string::npos) const noexcept
    {
        return m_string.rfind(_Char, _Off);
    }

    ionString substr(const size_type _Off = 0, const size_type _Count = std::string::npos) const noexcept
    {
        return ionString(m_string.substr(_Off, _Count));
    }

    int compare(const ionString& _other)
    {
        return m_string.compare(_other.m_string);
    }

	size_type find_first_of(char _Char, const size_type _Off = 0) const
	{
		return m_string.find_first_of(_Char, _Off);
	}

	size_type find_first_of(const ionString& _Char, const size_type _Off = 0) const
	{
		return m_string.find_first_of(_Char.m_string, _Off);
	}

    size_type find_last_of(char _Char, const size_type _Off = std::string::npos) const
    {
        return m_string.find_last_of(_Char, _Off);
    }

	size_type find_last_of(const ionString& _Char, const size_type _Off = std::string::npos) const
	{
		return m_string.find_last_of(_Char.m_string, _Off);
	}

    ionString& operator+=(const ionString& _Right)
    {	
        return (append(_Right));
    }

    //     eosString<HeapAllocatorString>& operator*() { return m_string; }
//     eosString<HeapAllocatorString> operator->() { return m_string; }
// 
//     const eosString<HeapAllocatorString>& operator*() const { return m_string; }
//     const eosString<HeapAllocatorString> operator->() const { return m_string; }



private:
    friend ION_DLL ionString operator+ (const ionString& _a, const ionString& _b);
    friend ION_DLL bool operator< (const ionString& _a, const ionString& _b);
    friend ION_DLL bool operator> (const ionString& _a, const ionString& _b);
    friend ION_DLL std::ostream& operator<<(std::ostream& os, const ionString& _value);
    friend ION_DLL bool operator==(const ionString& _Left, const ionString& _Right);

    eosString<HeapAllocatorString> m_string;
};

ION_DLL ionString operator+(const ionString& _a, const ionString& _b);
ION_DLL std::ostream& operator<<(std::ostream& os, const ionString& _value);
ION_DLL bool operator< (const ionString& _a, const ionString& _b);
ION_DLL bool operator> (const ionString& _a, const ionString& _b);
ION_DLL bool operator==(const ionString& _Left, const ionString& _Right);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename T> class ionSmartPointer;
template<typename T> using ionObjectHandler = ionSmartPointer<T>;

class Node;
typedef ionObjectHandler<Node> ObjectHandler;

template<typename T>
class ionSmartPointer final
{
private:
    template<typename U>
    friend class ionSmartPointer;

public:
    ionSmartPointer() : m_smartPtr(MemoryManager::Instance().GetSmartPointerAllocator()) {}

//     ionObjectHandler<Node>& Base()
//     {
//         return dynamic_cast<ionObjectHandler<Node>>(m_smartPtr.GetPtr());
//     }

    ionSmartPointer(T* _value) : m_smartPtr(MemoryManager::Instance().GetSmartPointerAllocator(), _value)
    {
        
    }

    template <typename U>
    ionSmartPointer(const ionSmartPointer<U>& _other) : m_smartPtr(_other.m_smartPtr)
    {
    }

    bool operator==(const ionSmartPointer<T>& _other) const 
    {
        return m_smartPtr.GetPtr() == (_other());
    }
//     template <typename U>
//     ionSmartPointer & operator=(const ionSmartPointer<U>& _other)
//     {
//         ionSmartPointer(_other).Swap(*this);
//         return *this;
//     }

//     ionSmartPointer & operator=(ionSmartPointer && _other)
//     {
//         eosSmartPointer(std::forward<eosSmartPointer>(_other)).Swap(*this);
//         return *this;
//     }
// 
//     ionSmartPointer & operator=(T* _other)
//     {
//         eosSmartPointer(_other).Swap(*this);
//         return *this;
//     }


public:
    eosSmartPointer<T, HeapAllocatorSmartPointer>& operator*() { return *m_smartPtr; }
    eosSmartPointer<T, HeapAllocatorSmartPointer>* operator->() { return &m_smartPtr; }

    const eosSmartPointer<T, HeapAllocatorSmartPointer>& operator*() const { return *m_smartPtr; }
    const eosSmartPointer<T, HeapAllocatorSmartPointer>* operator->() const { return &m_smartPtr; }

    T* operator()() const
    {
        return m_smartPtr.GetPtr();
    }

private:
    //friend bool operator==(const ionSmartPointer<T>& _Left, const ionSmartPointer<T>& _Right);

private:
    eosSmartPointer<T, HeapAllocatorSmartPointer> m_smartPtr;
};

// template<typename T>
// bool operator==(const ionSmartPointer<T>& _Left, const ionSmartPointer<T>& _Right)
// {
//     return (_Left.m_smartPtr == _Right.m_smartPtr);
// }


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// class ionIStringStream final
// {
// public:
//     ionIStringStream() {}
// 
// public:
//     eosIStringStream<HeapAllocator>& operator*() { return m_stream; }
//     eosIStringStream<HeapAllocator>* operator->() { return &m_stream; }
// 
// private:
//     eosIStringStream<HeapAllocator> m_stream;
// };
// 



ION_NAMESPACE_END



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define ionNewAligned(Type, Allocator, Alignment, ...)  eosNewAligned(Type, Allocator, Alignment, __VA_ARGS__)
#define ionNew(Type, ...)                               ionNewAligned(Type, ion::MemoryManager::Instance().GetGenericAllocator(), ION_MEMORY_ALIGNMENT_SIZE, __VA_ARGS__)
#define ionDelete(Object)                               eosDelete(Object, ion::MemoryManager::Instance().GetGenericAllocator())

#define ionNewRaw(Size)                                 (ion::MemoryManager::Instance().GetGenericAllocator()->Allocate(Size, ION_MEMORY_ALIGNMENT_SIZE, EOS_MEMORY_SOURCE_ALLOCATION_INFO))
#define ionDeleteRaw(Object)                            (ion::MemoryManager::Instance().GetGenericAllocator()->Free(Object))

#define ionNewDynamicArray(Type, Count)     eosNewDynamicArray(Type, Count, ion::MemoryManager::Instance().GetGenericAllocator())
#define ionNewArray(Type)                   eosNewArray(Type, ion::MemoryManager::Instance().GetGenericAllocator())
#define ionDeleteArray(ObjectArray)         eosDeleteArray((ObjectArray), (ion::MemoryManager::Instance().GetGenericAllocator()))




////////////////////////////////////////////////////////////////
//                      STD OVERLOAD
////////////////////////////////////////////////////////////////

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