#pragma once
#pragma comment(lib, "odbc32.lib")

// boost asio warning 
// https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10 / 11 이상을 타겟으로 설정
#endif

#include <stdint.h>
#include <boost/asio.hpp>
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <iostream>
#include <utility>
#include "util.h"

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include "macro.h"
#include "error.hpp"
#include "singleton.h"

#include <deque>
#include <queue>
#include <map>
#include <ppl.h>
#include <atomic>
#include <thread>

#include "object_pool.hpp"
#include "feature.h"

#include <flatbuffers/flatbuffers.h>
#include "enum_game_generated.h"
#include "enum_error_generated.h"
#include "enum_common_generated.h"
#include "common_generated.h"