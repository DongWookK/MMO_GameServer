#pragma once
#include "pch.h"
#include "session.h"
#include "boost/asio.hpp"
#include "packet_dispatcher.h"

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

		// 클라이언트에서 header.size에 (헤더 + 바디) 전체 크기를 넣어 보내줌
		size_t total_packet_size = header.size;

		if (total_packet_size < sizeof(packet_header) || total_packet_size > 8192) {
			std::cout << "Invalid packet size or overflow attack! Disconnecting..." << std::endl;
			socket_.close();
			return;
		}

		if (ring_buffer_.get_size() < total_packet_size) {
			return;
		}

		size_t body_size = total_packet_size - sizeof(packet_header);

		// (링버퍼에서 헤더 다음 위치부터 읽음)
		std::vector<uint8_t> body_buffer(body_size);
		if (body_size > 0) {
			ring_buffer_.read_contiguose(body_buffer.data(), sizeof(packet_header), body_size);
		}

		on_packet_received(header, body_buffer.data(), body_size);
		ring_buffer_.consume(total_packet_size);
	}
}

auto session::on_packet_received(const packet_header& header, const uint8_t* body_ptr, size_t body_size) -> void
{
	packet_dispatcher::instance()->dispatch(shared_from_this(), header.type, body_ptr, body_size);
}

void session::send(const flatbuffers::FlatBufferBuilder& builder) {
	send(builder.GetBufferPointer(), builder.GetSize());
}

void session::send(const uint8_t* data, size_t size) {
	if (!data || size == 0) return;

	std::vector<uint8_t> buffer(data, data + size);

	// send_queue 스레드세이프?? 검토해볼것
	send_queue_.push(std::move(buffer));

	if (!is_writing_) {
		is_writing_ = true;
		do_write();
	}
}

void session::do_write() {
	asio::async_write(
		socket_,
		asio::buffer(send_queue_.front()),
		[this, self = shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			if (!ec) {
				send_queue_.pop();

				if (!send_queue_.empty()) {
					do_write();
				}
				else {
					is_writing_ = false;
				}
			}
			else {
				socket_.close();
			}
		}
	);
}

auto session::get_socket() -> tcp_t::socket&
{
	return socket_;
}