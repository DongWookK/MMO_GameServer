#pragma once
#include "pch.h"

class main_server : public singleton<main_server>
{
public:
	using feature_ptr_t = std::shared_ptr<feature>;
	using feature_list_t = std::vector<feature_ptr_t>;

public:
	auto start_service() -> fw::error;
	auto stop_service() -> fw::error;

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
};