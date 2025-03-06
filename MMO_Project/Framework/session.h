#pragma once
#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;
using tcp_t = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp_t::socket socket) : socket_(std::move(socket)) {}

public:
    auto on_accept() -> void;
    auto get_socket() const -> tcp_t::socket&;

private:
    asio::streambuf input_{};
    tcp_t::socket socket_;
};