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

auto worker::allocate_job(io_context_t& io_context
						  , const db_conn_str_list_t& db_conn_str_list
						  , const sql_register_list_t& sql_register_list) -> void
{
	thread_ = std::jthread([this, &io_context, db_conn_str_list, sql_register_list]() {
		try {

			auto error_code = db_setup(db_conn_str_list, sql_register_list);
			ASSERT_RETURN(!error_code);

			FLOG_INFO("Thread {} ready to Start", std::this_thread::get_id()._Get_underlying_id());

			io_context.run();

			FLOG_INFO("Thread {} Stopped", std::this_thread::get_id()._Get_underlying_id());
			
#pragma region reset
			// todo : 나중에 해제하는것들 함수화
			fw::tls::db_list = nullptr;
#pragma endregion
		}
		catch (const std::exception& e) {
			FLOG_CRITICAL("Thread {} Error - {}", std::this_thread::get_id()._Get_underlying_id(), e.what());
		}
		});

	return;
}

auto worker::db_setup(const db_conn_str_list_t& db_conn_str_list, const sql_register_list_t& sql_register_list) -> fw::error
{
	for (uint8_t type = 0; type < fw::to_underlying(common::sql_type::MAX) + 1; ++type)
	{
		auto& sql = db_[type];
		if (!db_[type].connect(db_conn_str_list[type]))
		{
			FLOG_CRITICAL("WORKER ({}) DB Connection Failed", index_);
			ASSERT_RETURN_VALUE(false, error::code::sql_stmt_invalid);
		}
		
		sql_register_list[type](sql);

		auto error_code = sql.prepare();
		ASSERT_RETURN_VALUE(!error_code, error_code);

	}

	fw::tls::db_list = &db_;

	return error::code::ok;
}
