#pragma once

#include "pch.h"
#include "thread_worker.h"

/*---------------------------------------------------------------------
	Thread Manager
desc : worker_thread
warn : 
----------------------------------------------------------------------*/

static constexpr uint32_t G_thread_count = 4;

class thread_pool
{
private:
	using pool_t = boost::object_pool<worker>;

public:
	auto initialize() -> fw::error;
	auto acquire(uint32_t pThreadCount = G_thread_count) -> fw::error;

public:
	static auto set_up() -> fw::error;
	static auto start() -> fw::error;
	static auto stop() -> fw::error;
	static auto remove() -> fw::error;

private:
	pool_t pool_{};
	std::vector<worker> threads_{};
	bool is_setup_{};
};

class thread_manager : public singleton<thread_manager>
{
public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

public:
	thread_pool thread_pool_{};
};


