#pragma once
#include "pch.h"
#include "thread_worker.h"
#include "thread_manager.h"
#include "thread_local.h"

worker::worker()
{
}

auto worker::set_index(size_t index) -> void
{
	index_ = index;
}

auto worker::allocate_job(io_context_t& io_context) -> void
{
	thread_ = std::jthread([&io_context]() {
		try {
			std::cout << "[Thread " << std::this_thread::get_id() << "] Started.\n";
			io_context.run(); // 비동기 이벤트 루프 시작
			std::cout << "[Thread " << std::this_thread::get_id() << "] Stopped.\n";
		}
		catch (const std::exception& e) {
			std::cerr << "[Thread " << std::this_thread::get_id() << "] Exception: " << e.what() << "\n";
		}
		});

	return;
}