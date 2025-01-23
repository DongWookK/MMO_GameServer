#pragma once
#include "pch.h"
#include "thread_worker.h"
#include "thread_manager.h"
#include "thread_local.h"

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
	while (fw::thread_manager::instance()->is_on_service_)
	{
		// thread work
		std::thread::id this_id = std::this_thread::get_id();

		std::cout << "thread " << this_id << " sleeping...\n";

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}