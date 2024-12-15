#pragma once

#include "macro.h"
#include <boost/asio.hpp>

//multi-thread
#include <thread>
#include <process.h>
#include "thread_local.h"

//util
#include "singleton.h"

#include <ppl.h>
#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <deque>

#include "FwCObjectPool.h"

namespace fw
{
	using error = int32_t;

}