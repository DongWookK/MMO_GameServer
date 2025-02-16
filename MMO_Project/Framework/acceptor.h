#pragma once
#include "pch.h"

using namespace boost;

auto get_endpoint() -> const asio::ip::tcp::endpoint&
{
	static unsigned short port_num = 0221;
	asio::ip::address_v4 ip_address = asio::ip::address_v4::any();
	return asio::ip::tcp::endpoint(ip_address, port_num);
}