#pragma once

#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;

class session;
namespace fw
{
class network_manager : public singleton<network_manager>
{
public:
	static constexpr uint16_t port_no = 0221;
	static constexpr uint16_t BACKLOG_SIZE = 30;

	using session_ptr_t = std::shared_ptr<session>;

public:
	auto setup(asio::io_context* worker_context) -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
    auto start_accept() -> void;
	auto handle_accept(session_ptr_t new_session, boost::system::error_code error) -> void;

private:
	asio::ip::tcp::endpoint end_point_{};
	asio::io_context accept_context_{};
	boost::asio::io_context* worker_context_ = nullptr;
	asio::ip::tcp::acceptor acceptor_{ accept_context_ };
	std::jthread accept_thread_;
};
}
