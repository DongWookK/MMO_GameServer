#pragma once
#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;
using tcp_t = boost::asio::ip::tcp;

namespace asio = boost::asio;
using tcp_t = asio::ip::tcp;

// std::enable_shared_from_this를 상속받아 비동기 람다 안에서 세션 파괴 방지
class session : public std::enable_shared_from_this<session>
{
public:
	explicit session(asio::io_context& io_context) : socket_(io_context) {}

	auto set_index(size_t index) -> void;
	auto get_index() const -> size_t;
	auto on_accept() -> void;
	auto reset() -> void;
	auto get_socket() -> tcp_t::socket&;

private:
	auto read_from_socket() -> void;

private:
	tcp_t::socket socket_;
	size_t index_{ 0 };

	// 데이터 수신 버퍼
	static constexpr std::size_t BUFFER_SIZE = 8192;
	std::array<char, BUFFER_SIZE> buf_{};
};