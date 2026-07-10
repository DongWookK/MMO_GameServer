#pragma once
#include "pch.h"
#include "thread_manager.h"

class main_server : public singleton<main_server>
{
public:
	using feature_ptr_t = std::shared_ptr<feature>;
	using feature_list_t = std::vector<feature_ptr_t>;
	using worker_context_t = boost::asio::io_context;
	using work_guard_t = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	using strands_s_ptr_t = std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>>;

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

private:
	feature_list_t feature_list{};

	std::unique_ptr<worker_context_t> io_context_;
	std::unique_ptr<work_guard_t> work_guard_;
	std::unique_ptr<fw::thread_manager> thread_manager_{};
	std::vector<strands_s_ptr_t> strands_{};
};
