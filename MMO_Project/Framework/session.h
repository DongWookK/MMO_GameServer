#pragma once
#include "pch.h"
#include "boost/asio.hpp"

using namespace boost;
using tcp_t = boost::asio::ip::tcp;

#pragma pack(push, 1)
struct packet_header {
    uint16_t size; // 패킷 전체 크기 (헤더 + 바디)
    uint16_t type; // game::tr_type
};
#pragma pack(pop)

class ring_buffer {
public:
    ring_buffer(size_t capacity = 65536); // 64KB

    size_t get_size() const { return size_; }
    size_t get_free_space() const { return capacity_ - size_; }
    auto get_write_buffers() -> std::vector<asio::mutable_buffer>;
    auto on_write(size_t bytes) -> void;

    auto peek(void* dest, size_t len) const -> bool;	// 버퍼에서 데이터를 복사
    auto consume(size_t len) -> void;					// 파싱 완료 후 사용한 바이트만큼 헤드 이동 (Pop)
	auto read_contiguose(void* dest, size_t offset, size_t len) -> void;

private:
    std::vector<char> buffer_{};
    size_t capacity_;
    size_t head_; // 읽기 위치
    size_t tail_; // 쓰기 위치
    size_t size_;
};

class session : public std::enable_shared_from_this<session>
{
public:
	explicit session(asio::io_context& io_context) : socket_(io_context) {}

	auto set_index(size_t index) -> void;
	auto get_index() const -> size_t;
	auto on_accept() -> void;
	auto reset() -> void;
	auto get_socket() -> tcp_t::socket&;

private:
	auto read_from_socket() -> void;
	auto process_packet() -> void;
	auto on_packet_received(const packet_header& header, const uint8_t* body_ptr, size_t body_size) -> void;

public:
    auto send(const flatbuffers::FlatBufferBuilder& builder) -> void;
    auto send(const uint8_t* data, size_t size) -> void;
    auto do_write() -> void;

private:
	tcp_t::socket socket_;
	size_t index_{ 0 };
	
    ring_buffer ring_buffer_{};

    // todo - thread safe? atomic, concurrent queue,vector 활용 고민해볼것
    std::queue<std::vector<uint8_t>> send_queue_;
    bool is_writing_{};
};