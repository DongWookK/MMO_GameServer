#include "pch.h"
#include "session.h"
#include "boost/asio.hpp"

auto session::set_index(size_t index) -> void
{
	index_ = index;
}

auto session::get_index() const -> size_t
{
	return index_;
}

auto session::on_accept() -> void
{
	std::cout << "client connected " << std::endl;
	read_from_socket();
}

auto session::reset() -> void
{
	boost::system::error_code ec;
	if (socket_.is_open())
	{
		socket_.close(ec);
	}
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
