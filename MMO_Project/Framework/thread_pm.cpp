#include "pch.h"
#include "types.h"
#include "thread_pm.h"
#include "thread_manager.h"

auto thread_pm::start() -> fw::error
{
	fw::error error = 0;

	error = thread_manager::This()->start();
	if (0 < error)
	{
		// CONSOLE_SYS
	}

	return;
}
