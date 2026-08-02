#pragma once
#include "pch.h"

class session;
struct packet_header;

class packet_dispatcher : public singleton<packet_dispatcher>
{
public:
using handler_t = std::function<void(std::shared_ptr<session>, const uint8_t* body, size_t size)>;

public:
    auto register_handler(uint16_t packet_id, handler_t handler) -> void;
    auto dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void;

private:
    std::vector<handler_t> handlers_;
};