#pragma once
#include "pch.h"

class worker
{
public:
	using io_context_t = boost::asio::io_context;

public:
	worker();

public:
	auto set_index(uint32_t index) -> void;
	auto allocate_job(io_context_t& io_contex) -> void;

private:
	uint32_t index_;
	std::jthread thread_;
};