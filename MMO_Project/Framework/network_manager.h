#pragma once

#include "pch.h"
#include "boost/asio.hpp"
#include "session.h"

using namespace boost;

namespace fw
{
class network_manager : public singleton<network_manager>
{
public:
	using pool_t = fw::CObjectPool<session>;
	using object_t = pool_t::Object;
	
	static constexpr uint16_t port_no = 0221;
	static constexpr uint16_t BACKLOG_SIZE = 30;

	using session_ptr_t = std::shared_ptr<session>;

public:
	auto setup(asio::io_context* worker_context) -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

private:
	auto initialize_acceptor(asio::io_context* worker_context) -> fw::error;
	auto initialize_session_pool() -> fw::error;

    auto start_accept() -> void;
	auto handle_accept(session_ptr_t new_session, boost::system::error_code error) -> void;

private:
	asio::ip::tcp::endpoint end_point_{};
	asio::io_context accept_context_{};
	asio::ip::tcp::acceptor acceptor_{ accept_context_ };
	std::jthread accept_thread_;
	pool_t session_pool_;

	boost::asio::io_context* worker_context_ = nullptr;
};
}
