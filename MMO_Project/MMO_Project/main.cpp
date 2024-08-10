#include "pch.h"
#include "thread_manager.h"

int main()
{
	thread_manager::This()->setup();
	thread_manager::This()->start();
	system("pause");
	return 0;
}