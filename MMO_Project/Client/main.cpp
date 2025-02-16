#pragma once
#include "pch.h"

using namespace boost;

int main()
{
	// point Client to Server's endpoint
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::make_address_v4(raw_ip_address);

	if (ec.value() != 0)
	{
		std::cout << "Failed to parse the IP address. Error Code = " << ec.value() << ". Message:: " << ec.message();
		return ec.value();
	}

	asio::ip::tcp::endpoint ep(ip_address, port_num);

	return 0;
}