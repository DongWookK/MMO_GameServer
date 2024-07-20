#include "pch.h"
#include "thread_pm.h"

int main()
{
	thread_pm::This()->Start();
	system("pause");
	return 0;
}