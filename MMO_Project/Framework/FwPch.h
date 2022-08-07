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

//util
#include <chrono>
#include "Singleton.h"
#include "rapidjson/document.h"

#include <ppl.h>

#include "CThreadMgr.h"
#include "CThreadBase.h"
#include "FwCObjectPool.h"



#include <ppl.h>
#include <concurrent_vector.h>
#include <array>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>