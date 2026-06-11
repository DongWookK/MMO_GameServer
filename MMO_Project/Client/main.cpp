#pragma once
#include "pch.h"

using namespace boost;
using namespace std;

static const std::string raw_ip_address = "127.0.0.1";
static constexpr uint16_t port_no = 0221;

void write_to_socket(asio::ip::tcp::socket& sock)
{
	flatbuffers::FlatBufferBuilder builder;

	// 1. ���ڿ� �����͸� FlatBuffer ���ο� ����
	auto str_data = builder.CreateString("hello");

	// 2. TestEcho ��ü ���� �� ��� ���� (��Ű�� ������ �°� ȣ��)
	auto echo_offset = Game::CreateTestEcho(builder, std::to_underlying(Game::tr_type::TestEcho), str_data);
	builder.Finish(echo_offset);

	const uint8_t* data = builder.GetBufferPointer();
	int level = builder.GetSize();

	std::string buf = "hello";
	//������ Ÿ���� �����ؾ�����?
	
	// ���� ����, ������ ���۵� ���Ŀ� �����Ѵ�.
	asio::write(sock, asio::buffer(buf));
}

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

	std::string message{};
	bool out = true;
	cout << "client ready" << endl;
	
	cin >> message;

	try
	{
		sock.connect(ep); // �������̴�. ����ɶ����� thread stop
	}
	catch (system::system_error& e)
	{
		std::cout << "Error occured! Error Code =" << e.code() << ". Message: " << e.what() << endl;
	}

	while (out)
	{
		cin >> message;

	if ("disconnect" == message)
	{
		out = false;
	}
	if ("send" == message)
	{
		write_to_socket(sock);
	}
	else
	{
		std::cout << "invalid command" << endl;
	}

	}


	
	std::cout << "~client terminated" << endl;
	
	return 0;
}