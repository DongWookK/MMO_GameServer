#pragma once
#include "pch.h"

using namespace boost;

int main()
{
#pragma region get_endpoint
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
#pragma endregion
	
#pragma region create active socket

	// boost.asio의 핵심
	// 이 클래스의 객체를 통해 운영체제가 제공하는 네트워크 I/O 서비스로 접근할 수 있다.
	asio::io_context ioc;

	// 프로토콜을 표현하는 값들을 갖는 데이터 구조체역할. 기능은 제공하지 않는다.
	asio::ip::tcp protocol = asio::ip::tcp::v4();

	// instantiate active tcp socket
	asio::ip::tcp::socket sock(ioc);
	sock.open(protocol, ec); // 실제 운영체제 소켓이 할당된다.

	if (ec.value() != 0)
	{
		std::cout << "Failed to open socket. Error Code = " << ec.value() << ". Message:: " << ec.message();
		return ec.value();
	}
#pragma endregion

	

	
	
	return 0;



}