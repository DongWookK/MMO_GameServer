#pragma once


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
#include "error.hpp"
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

namespace fw
{
	enum flag
	{
		none
		, setup
		, start
		, stop
		, teardown
	};

}