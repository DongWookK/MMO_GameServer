#include "pch.h"
#include "network_manager.h"
#include "session.h"
#include "boost/bind/bind.hpp"

auto fw::network_manager::setup() -> fw::error
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

	acceptor_.listen(BACKLOG_SIZE);

	try 
	{
		start_accept();
	}
	catch(system::system_error &e)
	{
		std::cout << "server accept fail! Error code = << = " << e.code() << ". Message: " << e.what();
	}

	return fw::error{};
}

auto fw::network_manager::start() -> fw::error
{
	try
	{
		std::cout << "context_ run" << std::endl;
		context_.run(); // event loop start (ex. network_manager::start_accept())
	}
	catch (system::system_error& e)
	{
		std::cout << "context_ run fail.  Error code = << = " << e.code() << ". Message: " << e.what();
	}

	return fw::error{};
}


auto fw::network_manager::start_accept() -> void
{
	asio::ip::tcp::socket sock(context_);
	auto new_session = std::make_shared<session>(std::move(sock));

	acceptor_.async_accept(new_session->get_socket()
						   , boost::bind(&network_manager::handle_accept, this, new_session, boost::asio::placeholders::error));
}

auto fw::network_manager::handle_accept(session_ptr_t new_session, boost::system::error_code error) -> void
{
	if (!error)
	{
		new_session->on_accept();
		// todo session list¿¡ ³Ö±â.
	}
	else
	{
		std::cout << "handle_accept fail." << std::endl;
		std::cout << error.what() << error.message() << std::endl;

		new_session.reset();
		start_accept();
	}
}
