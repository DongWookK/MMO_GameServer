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

	// boost.asio�� �ٽ�
	// �� Ŭ������ ��ü�� ���� �ü���� �����ϴ� ��Ʈ��ũ I/O ���񽺷� ������ �� �ִ�.
	asio::io_context ioc;

	// ���������� ǥ���ϴ� ������ ���� ������ ����ü����. ����� �������� �ʴ´�.
	asio::ip::tcp protocol = asio::ip::tcp::v4();

	// instantiate active tcp socket
	asio::ip::tcp::socket sock(ioc);
	sock.open(protocol, ec); // ���� �ü�� ������ �Ҵ�ȴ�.

	if (ec.value() != 0)
	{
		std::cout << "Failed to open socket. Error Code = " << ec.value() << ". Message:: " << ec.message();
		return ec.value();
	}
#pragma endregion

	

	
	
	return 0;



}