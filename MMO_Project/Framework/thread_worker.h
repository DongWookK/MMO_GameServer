#pragma once
#include "pch.h"

class worker
{
public:
	using io_context_t = boost::asio::io_context;

public:
	worker();

public:
	auto allocate_job(io_context_t& io_contex) -> void;

private:
	static auto job() -> void;

private:
	std::jthread thread_;
};