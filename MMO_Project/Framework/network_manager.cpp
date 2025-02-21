#include "pch.h"
#include "network_manager.h"

auto network_manager::setup() -> fw::error
{
	boost::system::error_code ec;

	//open acceptor socket
	static const asio::ip::tcp protocol = asio::ip::tcp::v4();
	acceptor_ = asio::ip::tcp::acceptor(context_);
	acceptor_.open(protocol, ec);

	if (0 != ec.value())
	{
		std::cout << "failed to open the acceptor socket Error Code = " << ec.value();
		return ec.value();
	}

	//make endpoint
	asio::ip::address_v4 ip_address = asio::ip::address_v4::any();
	end_point_ = asio::ip::tcp::endpoint(ip_address, port_no);

	//bind
	acceptor_.bind(end_point_, ec);
	if (ec.value() != 0)
	{
		std::cout << "Failed to abind the acceptor socket. Error_code = " << ec.value() << std::endl;
		return ec.value();
	}

	const uint16_t BACKLOG_SIZE = 30; // 대기열 연결요청 최대 수
	acceptor_.listen(BACKLOG_SIZE);

	asio::ip::tcp::socket sock(context_);
	try 
	{
		acceptor_.accept(sock);
	}
	catch(system::system_error &e)
	{
		std::cout << "server accept fail! Error code = << = " << e.code() << ". Message: " << e.what();
	}
	
	return fw::error{};
}
