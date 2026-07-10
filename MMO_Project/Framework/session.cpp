#include "pch.h"
#include "session.h"
#include "boost/asio.hpp"

session::session(boost::asio::io_context* io_context)
	: socket_(*io_context)
{
}

auto session::on_accept() -> void
{
	std::cout << "client connected " << std::endl;
	read_from_socket();
}

auto session::read_from_socket() -> std::string
{
	asio::read(socket_, asio::buffer(buf_, message_size));
	return std::string(buf_.data(), message_size);
}

auto session::get_socket() -> tcp_t::socket&
{
	return socket_;
}
