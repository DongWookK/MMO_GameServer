#include "pch.h"
#include "thread_pm.h"
#include "thread_manager.h"

void thread_pm::Start(void)
{
	uint32 aErrorCode = 0;

	aErrorCode = thread_manager::This()->Open();
	if (0 < aErrorCode)
	{
		// CONSOLE_SYS
	}

	return;
}
