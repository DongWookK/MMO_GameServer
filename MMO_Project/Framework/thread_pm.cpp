#include "pch_fw.h"
#include "thread_pm.h"
#include "thread_manager.h"

worker::worker()
{
	std::cout << "my id is %d" << thread_Id_ << std::endl;
}

auto worker::allocate_job() -> const uint32_t
{
	thread_ = std::thread(job);
	return 0;
}

auto worker::job() -> const uint32_t
{
	while (Flag::START == thread_manager::instance()->flag_)
	{
		// thread work
		std::thread::id this_id = std::this_thread::get_id();

		std::cout << "thread " << this_id << " sleeping...\n";

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}

auto thread_pm::start() -> fw::error
{
	fw::error error = 0;

	error = thread_manager::instance()->start();
	if (0 < error)
	{
		// CONSOLE_SYS
	}

	return error;
}
