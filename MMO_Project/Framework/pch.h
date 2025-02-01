#pragma once


// boost asio warning 
// https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#define _WIN32_WINNT 0x0A000000	// win10

#include <stdint.h>
#include <boost/asio.hpp>
#include <iostream>


#include "macro.h"
#include "singleton.h"

#include <deque>
#include <map>
#include <ppl.h>
#include <atomic>
#include <thread>

#include "object_pool.hpp"
#include "feature.h"

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