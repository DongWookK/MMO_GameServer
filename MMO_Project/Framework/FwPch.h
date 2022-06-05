#pragma once

#include "Types.h"
#include "FwGlobal.h"
#include "FwTLS.h"
#include "FwMacro.h"
#include "Container.h"


//win
#include <windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


//multi-thread
#include <thread>
#include <mutex>
#include <atomic>

//util
#include <chrono>

#include "Singleton.h"
#include "rapidjson/document.h"


