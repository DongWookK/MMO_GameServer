#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\Framework.lib")
#else
#pragma comment(lib, "Release\\Framework.lib")
#endif

// boost asio warning 
// https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#define _WIN32_WINNT 0x0A000000	// win10

#include <boost/asio.hpp>
#include <iostream>

#include <flatbuffers/flatbuffers.h>
#include "game_generated.h"
