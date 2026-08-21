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
			FLOG_INFO("Thread {} Started", std::this_thread::get_id()._Get_underlying_id());
			io_context.run(); // 비동기 이벤트 루프 시작
			FLOG_INFO("Thread {} Stopped", std::this_thread::get_id()._Get_underlying_id());
		}
		catch (const std::exception& e) {
			FLOG_CRITICAL("Thread {} Error - {}", std::this_thread::get_id()._Get_underlying_id(), e.what());
			//std::cerr << "[Thread " << std::this_thread::get_id() << "] Exception: " << e.what() << "\n";
		}
		});

	return;
}