#pragma once


#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Framework\\Debug\\Framework.lib")
#else
#pragma comment(lib, "Framework\\Release\\Framework.lib")
#endif


#include <iostream>
#include "FwPch.h"
using namespace std;