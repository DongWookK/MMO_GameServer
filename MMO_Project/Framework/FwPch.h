#pragma once

#include "Types.h"
#include "FwGlobal.h"
#include "FwTLS.h"
#include "FwMacro.h"
#include "FwCContainer.h"


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
#include "Singleton.h"

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