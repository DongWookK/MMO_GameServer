#pragma once

#include "pch.h"
#include "thread_worker.h"

namespace fw
{

static constexpr uint32_t G_thread_count = 4;

class thread_pool
{
public:
	using pool_t = fw::CObjectPool<worker>;
	using worker_shd_t = std::shared_ptr<worker>;

public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
	auto initialize() -> fw::error;
	auto acquire(uint32_t pThreadCount = G_thread_count) -> fw::error;

	static auto setup_worker() -> fw::error;
	static auto teardown_worker() -> fw::error;

private:
	pool_t pool_{};
	std::vector<worker_shd_t> threads_{};
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
	auto get_flag() const->fw::flag;

private:
	fw::flag flag_{};
	thread_pool thread_pool_{};

};
};
