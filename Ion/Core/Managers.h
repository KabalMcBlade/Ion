#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN


ION_DLL void InitializeManagers();
ION_DLL void ShutdownManagers();


ION_NAMESPACE_END