#include "UUID.h"

#include "../Utilities/Tools.h"
#include "../App/Window.h"

ION_NAMESPACE_BEGIN

ionU32 UUID::g_counter = 0;

void UUID::GenerateUUID()
{
    ionU32 currTime = static_cast<ionU32>(std::time(nullptr));
    std::srand(currTime);

    m_timeStamp = currTime;
    m_macAddressHigh = Window::s_physicalAddressNumHigh;
    m_randomNumber = std::rand();
    m_index = g_counter;

    ++g_counter;

    // generate to string
    ionString hight = std::to_string(m_highBites).c_str();
    ionString low = std::to_string(m_lowBites).c_str();

    m_uuidString = hight.append(low);
}


ION_NAMESPACE_END