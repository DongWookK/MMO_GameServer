#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\Framework.lib")
#else
#pragma comment(lib, "Release\\Framework.lib")
#endif

// boost asio warning 
// https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10 / 11 이상을 타겟으로 설정
#endif

#include <stdint.h>
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <utility>

#include "macro.h"
#include "singleton.h"

#include <deque>
#include <map>
#include <ppl.h>
#include <atomic>
#include <thread>

#include "object_pool.hpp"
#include "feature.h"

#include <flatbuffers/flatbuffers.h>
#include "enum_game_generated.h"