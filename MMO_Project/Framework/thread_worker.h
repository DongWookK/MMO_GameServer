#pragma once
#include "pch.h"
#include "db_manager.h"

class worker
{
public:
	using io_context_t = boost::asio::io_context;

public:
	worker();

public:
	auto set_index(size_t index) -> void;
	auto allocate_job(io_context_t& io_contex, const std::wstring& db_connection_str, std::function<fw::error(db_manager&)> on_init) -> void;
	auto get_db() -> db_manager& { return db_; }

	auto db_setup(const std::wstring& db_connection_str, std::function<fw::error(db_manager&)> on_init) -> fw::error;

private:
	size_t index_;
	std::jthread thread_;
	db_manager db_;
};