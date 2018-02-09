// Ion.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Ion.h"



ION_NAMESPACE_BEGIN


Linkage::Linkage() : m_iCounterLinks(0) { IncrementLinkage(); }

void Linkage::IncrementLinkage()
{
    ++m_iCounterLinks;
}


ION_NAMESPACE_END
