#pragma once

#include "types.h"
#include "global.h"
#include "thread_local.h"
#include "macro.h"
#include "FwCContainer.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//win
#include <windows.h>
#include <iostream>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


//multi-thread
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <functional>
#include <process.h>

//util
#include "singleton.h"

#include <ppl.h>
#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <concrt.h>

#undef max
#include <concurrent_vector.h>

#include "FwCObjectPool.h"