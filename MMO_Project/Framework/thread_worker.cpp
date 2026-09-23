#pragma once
#include "pch.h"
#include "thread_worker.h"
#include "thread_manager.h"
#include "thread_local.h"
#include "db_manager.h"

worker::worker()
{
}

auto worker::set_index(size_t index) -> void
{
	index_ = index;
}

auto worker::allocate_job(io_context_t& io_context, const std::wstring& db_connection_str, std::function<fw::error(db_manager&)> on_init) -> void
{
	thread_ = std::jthread([this, &io_context, db_connection_str, on_init]() {
		try {
			FLOG_INFO("Thread {} Started", std::this_thread::get_id()._Get_underlying_id());

			auto error_code = db_setup(db_connection_str, on_init);
			ASSERT_RETURN(!error_code);

			io_context.run();

			FLOG_INFO("Thread {} Stopped", std::this_thread::get_id()._Get_underlying_id());
		}
		catch (const std::exception& e) {
			FLOG_CRITICAL("Thread {} Error - {}", std::this_thread::get_id()._Get_underlying_id(), e.what());
		}
		});

	return;
}

auto worker::db_setup(const std::wstring& db_connection_str, std::function<fw::error(db_manager&)> on_init) -> fw::error
{
	if (!db_.connect(db_connection_str)) {
		FLOG_CRITICAL("WORKER ({}) DB Connection Failed", index_);
		ASSERT_RETURN_VALUE(false, error::code::sql_stmt_invalid);
	}

	auto error_code = fw::error{};
	
	error_code = on_init(db_);
	ASSERT_RETURN_VALUE(!error_code, error_code);
	
	error_code = db_.prepare();
	ASSERT_RETURN_VALUE(!error_code, error_code);

	return error_code;
}
