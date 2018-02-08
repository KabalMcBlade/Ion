// Ion.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Ion.h"


// This is an example of an exported variable
ION_API int nIon=0;

// This is an example of an exported function.
ION_API int fnIon(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see Ion.h for the class definition
CIon::CIon()
{
    return;
}
