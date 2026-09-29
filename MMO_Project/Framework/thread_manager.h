#pragma once

#include "pch.h"
#include "thread_worker.h"

namespace fw
{
class thread_pool
{
public:
	using pool_t = fw::CObjectPool<worker>;
	using object_t = pool_t::Object;
	using worker_shd_t = std::shared_ptr<worker>;
	using io_context_t = boost::asio::io_context;
	using db_conn_str_list_t = worker::db_conn_str_list_t;
	using sql_register_t = worker::sql_register_t;
	using sql_register_list_t = worker::sql_register_list_t;

public:
	auto setup(const uint32_t thread_count) -> fw::error;
	auto start(io_context_t& io_context
			   , const db_conn_str_list_t& db_conn_str_list
			   , const sql_register_list_t& sql_register_list) -> fw::error;
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

class thread_manager
{
public:
	using io_context_t = boost::asio::io_context;
	using db_conn_str_list_t = thread_pool::db_conn_str_list_t;
	using sql_register_t = thread_pool::sql_register_t;
	using sql_register_list_t = thread_pool::sql_register_list_t;

	thread_manager() = delete;
	thread_manager(io_context_t* io_context, uint32_t thread_count);

public:
	auto setup() -> fw::error;
	auto start(const db_conn_str_list_t& db_conn_str_list, const sql_register_list_t& sql_register_list) -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

public:
	std::atomic_bool is_on_service_;

private:
	io_context_t& io_context_;
	uint32_t thread_count_{};
	thread_pool thread_pool_{};
};
};
