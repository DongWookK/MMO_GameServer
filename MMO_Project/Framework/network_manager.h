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
	static constexpr uint16_t BACKLOG_SIZE = 30; // 대기열 연결요청 최대 

	using session_ptr_t = std::shared_ptr<session>;

public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
    auto start_accept() -> void;
	auto handle_accept(session_ptr_t new_session, boost::system::error_code error) -> void;

private:
	asio::ip::tcp::endpoint end_point_{};
	asio::io_context context_{};
	asio::ip::tcp::acceptor acceptor_{ context_ };
};
}
