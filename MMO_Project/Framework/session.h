#pragma once
#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;
using tcp_t = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
    static constexpr size_t message_size = 10;

public:
    session(tcp_t::socket socket) : socket_(std::move(socket)) {}

public:
    auto on_accept() -> void;
    auto read_from_socket() -> std::string;
    
public:
    auto get_socket() -> tcp_t::socket&;

private:
    std::array<char, message_size> buf_{};
    tcp_t::socket socket_;
};