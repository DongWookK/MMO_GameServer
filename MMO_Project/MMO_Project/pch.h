#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Framework\\Debug\\Framework.lib")
#else
#pragma comment(lib, "Framework\\Release\\Framework.lib")
#endif

int main2();