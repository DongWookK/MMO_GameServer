#pragma once
#include "pch.h"
#include "session.h"
#include "boost/asio.hpp"

ring_buffer::ring_buffer(size_t capacity)
	: buffer_(capacity), capacity_(capacity), head_(0), tail_(0), size_(0) 
{}

auto ring_buffer::get_write_buffers() -> std::vector<asio::mutable_buffer>
{
	{
		size_t write_pos = tail_;
		size_t free_space = get_free_space();

		if (write_pos + free_space <= capacity_) {					// 경계선을 넘지 않는 단일 구간
			return std::vector<asio::mutable_buffer>{
				asio::buffer(&buffer_[write_pos], free_space)
			};
		}
		else {
			size_t first_part = capacity_ - write_pos;
			size_t second_part = free_space - first_part;
			return std::vector<asio::mutable_buffer>{
				asio::buffer(&buffer_[write_pos], first_part),
					asio::buffer(&buffer_[0], second_part)			// 맨 앞에서 짤린 부분
			};
		}
	}
}

auto ring_buffer::on_write(size_t bytes) -> void
{
	tail_ = (tail_ + bytes) % capacity_;
	size_ += bytes;
}

auto ring_buffer::peek(void* dest, size_t len) const -> bool
{
	if (size_ < len) return false;

	size_t read_pos = head_;
	size_t first_part = std::min(len, capacity_ - read_pos);
	size_t second_part = len - first_part;

	std::memcpy(dest, &buffer_[read_pos], first_part);
	if (second_part > 0) {
		std::memcpy(static_cast<char*>(dest) + first_part, &buffer_[0], second_part);
	}
	return true;
}

auto ring_buffer::consume(size_t len) -> void
{
	if (len > size_) len = size_;
	head_ = (head_ + len) % capacity_;
	size_ -= len;
}

auto ring_buffer::read_contiguose(void* dest, size_t offset, size_t len) -> void
{
	size_t read_pos = (head_ + offset) % capacity_;
	size_t first_part = std::min(len, capacity_ - read_pos);
	size_t second_part = len - first_part;

	std::memcpy(dest, &buffer_[read_pos], first_part);
	if (second_part > 0) 
	{
		std::memcpy(static_cast<char*>(dest) + first_part, &buffer_[0], second_part);
	}
}

/*-----------------------------------------------------
*		session
-------------------------------------------------------*/

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
        ring_buffer_.get_write_buffers(),
        [this, self = shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                ring_buffer_.on_write(bytes_transferred);
                process_packet();
                read_from_socket();
            }
            else {
                socket_.close();
            }
        }
    );
}

auto session::process_packet() -> void
{
    while (true) {
        packet_header header;
        if (!ring_buffer_.peek(&header, sizeof(packet_header))) {
            return;
        }

        size_t total_packet_size = sizeof(packet_header) + header.body_size;

        if (total_packet_size > 8192) {
            std::cout << "Packet size overflow attack! Disconnecting..." << std::endl;
            socket_.close();
            return;
        }

        if (ring_buffer_.get_size() < total_packet_size) {
            return;
        }

        // note : FlatBuffers는 메모리가 연속적이어야 하므로 contiguose read
        std::vector<uint8_t> body_buffer(header.body_size);
        ring_buffer_.read_contiguose(body_buffer.data(), sizeof(packet_header), header.body_size);

        // todo : received 핸들러 main_server에서 설정
        on_packet_received(header, body_buffer.data(), header.body_size);


        ring_buffer_.consume(total_packet_size);
    }
}

auto session::get_socket() -> tcp_t::socket&
{
	return socket_;
}