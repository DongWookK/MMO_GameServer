#pragma once
#include "pch.h"
#include "thread_worker.h"
#include "thread_manager.h"
#include "thread_local.h"

worker::worker()
{
	//std::cout << "my id is %d" << thread_Id_ << std::endl;
}

auto worker::allocate_job() -> void
{
	thread_ = std::thread(job);
	return;
}

auto worker::job() -> void
{
	uint32_t this_id = 3;

	while (fw::thread_manager::instance()->is_on_service_.load())
	{
		//std::thread::id this_id = std::this_thread::get_id();
		std::cout << "thread " << this_id << " sleeping...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return;
}