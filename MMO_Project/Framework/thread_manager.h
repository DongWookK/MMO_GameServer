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
	using object_t = pool_t::Object;
	using worker_shd_t = std::shared_ptr<worker>;

public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
	auto initialize() -> fw::error;
	auto acquire(uint32_t thread_count = G_thread_count) -> fw::error;

	static auto setup_worker() -> fw::error;
	static auto teardown_worker() -> fw::error;

private:
	pool_t pool_{};
	std::vector<object_t> threads_{};
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

public:
	std::atomic_bool is_on_service_;

private:
	thread_pool thread_pool_{};

};
};
