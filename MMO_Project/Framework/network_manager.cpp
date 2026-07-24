#include "pch.h"
#include "network_manager.h"
#include "session.h"
#include "boost/bind/bind.hpp"

auto fw::network_manager::setup(asio::io_context* worker_context) -> fw::error
{
	auto error_code = fw::error{};

	error_code = initialize_acceptor(worker_context);
	ASSERT_RETURN_VALUE(!(error_code), error_code);
	
	error_code = initialize_session_pool();
	ASSERT_RETURN_VALUE(!(error_code), error_code);

	return error_code;
}

auto fw::network_manager::start() -> fw::error
{
	auto error_code = fw::error{};

	try
	{
		start_accept();
	}
	catch (system::system_error& e)
	{
		FLOG_ERROR("server accept fail! Error code({}), Msg({})", e.code().value(), e.what());
		error_code.assign(111);
	}

	accept_thread_ = std::jthread([this]() {
		try
		{
			std::cout << "context_ run" << std::endl;
			accept_context_.run();
		}
		catch (const std::exception& e)
		{
			FLOG_ERROR("accept_context run fail. Error ({})", e.what());
		}
	});

	return fw::error{};
}

auto fw::network_manager::initialize_acceptor(asio::io_context* worker_context) -> fw::error
{
	worker_context_ = worker_context;

	boost::system::error_code ec;

	static const asio::ip::tcp protocol = asio::ip::tcp::v4();
	acceptor_ = asio::ip::tcp::acceptor(accept_context_);
	acceptor_.open(protocol, ec);

	if (0 != ec.value())
	{
		std::cout << "failed to open the acceptor socket Error Code = " << ec.value();
		return fw::error{1};
	}

	asio::ip::address_v4 ip_address = asio::ip::address_v4::any();
	end_point_ = asio::ip::tcp::endpoint(ip_address, port_no);

	acceptor_.bind(end_point_, ec);
	if (ec.value() != 0)
	{
		std::cout << "Failed to abind the acceptor socket. Error_code = " << ec.value() << std::endl;
		return fw::error{ 1 };
	}

	// listen
	acceptor_.listen(BACKLOG_SIZE);

	return fw::error{};
}

auto fw::network_manager::initialize_session_pool() -> fw::error
{
	auto error_code = fw::error{};

	session_pool_.AllocateChunk<session>(
		[this]() { return std::make_unique<session>(*worker_context_); },
		[](session* s, size_t idx) { s->set_index(idx); return 0; },
		[](session* s) { s->reset(); return 0; },
		1000, true
	);

	return fw::error();
}

auto fw::network_manager::start_accept() -> void
{
	auto new_session = session_pool_.AcquireObject();
	ASSERT_RETURN(nullptr != new_session);

	acceptor_.async_accept(new_session->get_socket()
							, boost::bind(&network_manager::handle_accept, this, new_session, boost::asio::placeholders::error));
}

auto fw::network_manager::handle_accept(session_ptr_t new_session, boost::system::error_code error) -> void
{
	spdlog::info("network_manager::handel_accept....");

	if (!error)
	{
		new_session->on_accept();
		// TODO: session list에 넣기.
	}
	else
	{
		std::cout << "handle_accept fail. " << error.message() << std::endl;
		new_session.reset();
	}

	start_accept();
}