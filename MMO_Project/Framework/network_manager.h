#pragma once

#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;

class network_manager
{
public:
	static constexpr uint16_t port_no = 0221;

public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
	asio::ip::tcp::endpoint end_point_{};
	asio::ip::tcp::acceptor acceptor_;
	asio::io_context context_{};
};
