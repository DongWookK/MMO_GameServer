#pragma once
#include "pch.h"

using namespace boost;
using namespace std;

static const std::string raw_ip_address = "127.0.0.1";
static constexpr uint16_t port_no = 0221;

int main()
{
#pragma region get_endpoint
	// point Client to Server's endpoint

	boost::system::error_code ec;
	asio::ip::address ip_address = asio::ip::make_address_v4(raw_ip_address);

	if (ec.value() != 0)
	{
		std::cout << "Failed to parse the IP address. Error Code = " << ec.value() << ". Message:: " << ec.message();
		return ec.value();
	}

	asio::ip::tcp::endpoint ep(ip_address, port_no);
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

#pragma region connect server
	
#pragma endregion

	std::string message{};
	bool out = true;
	cout << "client ready" << endl;
	while (out)
	{
	cin >> message;
		
	if ("connect" == message)
	{
		try
		{
			sock.connect(ep); // �������̴�. ����ɶ����� thread stop
		}
		catch (system::system_error& e)
		{
			std::cout << "Error occured! Error Code =" << e.code() << ". Message: " << e.what() << endl;
		}
	}
	else if ("disconnect" == message)
	{
		out = false;
	}
	else
	{
		std::cout << "invalid command" << endl;
	}
	}
	
	std::cout << "~client terminated" << endl;
	
	return 0;
}