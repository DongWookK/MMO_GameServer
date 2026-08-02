#pragma once
#include "pch.h"
#include "packet_dispatcher.h"

auto packet_dispatcher::register_handler(uint16_t packet_id, handler_t handler) -> void
{
    if (handlers_.size() <= packet_id)
    {
        handlers_.resize(packet_id);
    }

	handlers_[packet_id] = std::move(handler);
}

auto packet_dispatcher::dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void
{
    ASSERT_RETURN(handlers_.size() <= packet_id);
    
    const auto& handler = handlers_[packet_id];
    if (handler) {
        handler(sess, body_data, body_size);
    }
    else {
        // 등록되지 않은 빈 슬롯 처리
    }
}
