#pragma once
#include "pch.h"
#include "thread_manager.h"
#include <nlohmann/json.hpp>

class db_manager;
class main_server : public singleton<main_server>
{
public:
	friend class singleton<main_server>;
public:
	main_server() = default;
	~main_server();

public:
	using feature_s_ptr_t = std::shared_ptr<feature>;
	using feature_list_t = std::vector<feature_s_ptr_t>;
	using worker_context_t = boost::asio::io_context;
	using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	using db_conn_str_list_t = std::array<std::wstring, fw::to_underlying(common::sql_type::MAX)>;
	using dbms_t = std::array<db_manager, fw::to_underlying(common::sql_type::MAX)>;
	using sql_registers_t = std::array<std::function<void(db_manager&)>, fw::to_underlying(common::sql_type::MAX)>;

public:
	auto start_service() -> fw::error;
	auto stop_service() -> fw::error;

	auto get_io_context() -> boost::asio::io_context*;

private:
	auto core_setup() -> fw::error;
	auto core_start() -> fw::error;
	auto core_stop() -> fw::error;
	auto core_teardown() -> fw::error;

	auto load_feature() -> void;
	auto feature_setup() -> fw::error;
	auto feature_start() -> fw::error;
	auto feature_stop() -> fw::error;
	auto feature_teardown() -> fw::error;

	auto on_service() -> fw::error;

	auto load_config_from_file() -> fw::error;
	auto set_thread_config() -> fw::error;
	auto set_network_config() -> fw::error;

	auto primary_thread_start() -> fw::error;
	auto thread_manager_start() -> fw::error;

private:
	feature_list_t feature_list_{};

	std::unique_ptr<worker_context_t> io_context_;
	std::unique_ptr<work_guard_t> work_guard_;
	std::unique_ptr<fw::thread_manager> thread_manager_{};

	db_conn_str_list_t db_conn_str_list_{};
	dbms_t dbms_{};
	sql_registers_t sql_reigsters_{};
};
