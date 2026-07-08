#pragma once

#include "pch.h"
#include "thread_worker.h"

namespace fw
{
class thread_pool
{
	using io_context_t = boost::asio::io_context;

public:
	using pool_t = fw::CObjectPool<worker>;
	using object_t = pool_t::Object;
	using worker_shd_t = std::shared_ptr<worker>;

public:
	auto setup(const uint32_t thread_count) -> fw::error;
	auto start(io_context_t& io_context) -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
	auto initialize(const uint32_t thread_count) -> fw::error;
	auto acquire() -> fw::error;

	static auto setup_worker() -> fw::error;
	static auto teardown_worker() -> fw::error;

private:
	uint32_t thread_count_{};
	std::vector<object_t> threads_{};
	pool_t pool_{};
};

class thread_manager : public singleton<thread_manager>
{
	using io_context_t = boost::asio::io_context;

public:
	auto setup(io_context_t& ioc, uint32_t thread_count) -> fw::error;
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
