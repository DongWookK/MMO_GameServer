#include "pch.h"
#include "session.h"

auto session::on_accept() -> void
{
	std::cout << "client connected " << std::endl;
	//do_read();
}

auto session::get_socket() const -> tcp_t::socket&
{
	return socket_;
}
