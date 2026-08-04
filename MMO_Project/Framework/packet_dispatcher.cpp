#pragma once
#include "pch.h"
#include "packet_dispatcher.h"

auto packet_dispatcher::dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void
{
    ASSERT_RETURN(handlers_.size() > packet_id);
    
    if (packet_id >= handlers_.size()) {
        return;
    }

    const auto& handler = handlers_[packet_id];
    if (handler) {
        handler(sess, body_data, body_size);
    }
    else {
        // todo : unsupport packet_id
    }
}
