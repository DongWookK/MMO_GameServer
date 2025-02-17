#pragma once
#include "pch.h"

using namespace boost;

auto get_acceptor() -> std::shared_ptr<asio::ip::tcp::acceptor>
{
	boost::system::error_code ec;

	asio::io_context ioc; // todo - conver to in-parameter variable

	asio::ip::tcp protocol = asio::ip::tcp::v4();
	auto acceptor = std::make_shared<asio::ip::tcp::acceptor>(ioc);
	acceptor->open(protocol, ec); // in this state, dosen't listen.
	
	if (0 != ec.value())
	{
		std::cout << "failed to open the acceptor socket Error Code = " << ec.value();
	}

	return acceptor;
}


auto get_endpoint() -> const asio::ip::tcp::endpoint&
{
	static unsigned short port_num = 0221;
	asio::ip::address_v4 ip_address = asio::ip::address_v4::any();
	return asio::ip::tcp::endpoint(ip_address, port_num);
}