#pragma once
#include "pch.h"
#include "db_manager.h"

class worker
{
public:
	using io_context_t = boost::asio::io_context;
	using db_conn_str_list_t = std::array<std::wstring, fw::to_underlying(common::sql_type::MAX) + 1>;
	using sql_register_t = std::function<void(db_manager&)>;
	using sql_register_list_t = std::array<sql_register_t, fw::to_underlying(common::sql_type::MAX) + 1>;
	using sqls_t = std::array<db_manager, fw::to_underlying(common::sql_type::MAX) + 1>;

public:
	worker();

public:
	auto set_index(size_t index) -> void;
	auto allocate_job(io_context_t& io_contex
					  , const db_conn_str_list_t& db_conn_str_list
					  , const sql_register_list_t& sql_register_list) -> void;

	auto db_setup(const db_conn_str_list_t& db_conn_str_list, const sql_register_list_t& sql_register_list) -> fw::error;

private:
	size_t index_;
	std::jthread thread_;
	sqls_t db_;
};