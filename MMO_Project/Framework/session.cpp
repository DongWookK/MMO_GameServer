#pragma once
#include "pch.h"
#include "session.h"
#include "boost/asio.hpp"

auto session::set_index(size_t index) -> void
{
	index_ = index;
}

auto session::get_index() const -> size_t
{
	return index_;
}

auto session::on_accept() -> void
{
	std::cout << "Client connected (Session index: " << index_ << ")" << std::endl;

	read_from_socket(); // 비동기 수신 루프 시작
}

auto session::reset() -> void
{
	boost::system::error_code ec;
	if (socket_.is_open())
	{
		socket_.close(ec);
	}
}

auto session::read_from_socket() -> void
{
	socket_.async_read_some(
		asio::buffer(buf_),
		[this, self = shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred) // 수신 중 파괴 방지를 위한 shared_from_this
		{
			if (!error)
			{
				std::string received_msg(buf_.data(), bytes_transferred);
				std::cout << "[Session " << index_ << "] Recv (" << bytes_transferred << " bytes): " << received_msg << std::endl;

				// TODO: 여기서 수신된 데이터를 패킷 단위로 조립하거나 핸들러로 전달

				read_from_socket();
			}
			else
			{
				if (error == asio::error::eof)
				{
					std::cout << "Client disconnected cleanly (Session index: " << index_ << ")" << std::endl;
					// TODO: 세션 삭제, 풀 반납 필요
				}
				else
				{
					std::cout << "Read fail. Error code: " << error.value() << ", Msg: " << error.message() << std::endl;
				}

				reset();
			}
		}
	);
}

auto session::get_socket() -> tcp_t::socket&
{
	return socket_;
}