#include "MemoryWrapper.h"


EOS_USING_NAMESPACE
ION_NAMESPACE_BEGIN


ionString operator+(const ionString& _a, const ionString& _b)
{
    ionString ac = _a;
    return ac.append(_b);
    //     const ionSize la = _a.m_string.length();
    //     const ionSize lb = _b.m_string.length();
    // 
    //     const ionSize totalLength = la + lb;
    // 
    //     char *newCharArray = new char[totalLength]{};
    //     strcat_s(newCharArray, la, _a.c_str());
    //     strcat_s(newCharArray, lb, _b.c_str());
    // 
    //     ionString newString(newCharArray);
    // 
    //     delete [] newCharArray;
    // 
    //     return newString;
}

std::ostream& operator<<(std::ostream& os, const ionString& _value)
{
    os << _value.c_str();
    return os;
}

bool operator< (const ionString& _a, const ionString& _b)
{
    return (_a.m_string.compare(_b.m_string) < 0);
}

bool operator> (const ionString& _a, const ionString& _b)
{
    return (_a.m_string.compare(_b.m_string) > 0);
}

bool operator==(const ionString& _Left, const ionString& _Right)
{
    return (_Left.equal(_Right));
}

ION_NAMESPACE_END