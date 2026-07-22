#pragma once
#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;
using tcp_t = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
    static constexpr size_t message_size = 10;

public:
    explicit session(boost::asio::io_context& worker_context)
        : socket_(worker_context)
        , index_(0)
    {}

    auto set_index(size_t index) -> void;
    auto get_index() const -> size_t;

    auto on_accept() -> void;
    auto reset() -> void;

    auto read_from_socket() -> std::string;

public:
    auto get_socket() -> tcp_t::socket&;

private:
    size_t index_;

    std::array<char, message_size> buf_{};
    tcp_t::socket socket_;
};