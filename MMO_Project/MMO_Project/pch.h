#pragma once


#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Framework\\Debug\\Framework.lib")
#else
#pragma comment(lib, "Framework\\Release\\Framework.lib")
#endif


#include <iostream>
#include "FwPch.h"
using namespace std;