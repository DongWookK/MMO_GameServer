#pragma once
#include "pch.h"
#include "packet_dispatcher.h"
#include "session.h"

auto packet_dispatcher::set_strands(boost::asio::io_context* io_context, size_t strand_count) -> void
{
    for (uint32_t i = 0; i < strand_count; ++i)
    {
        strands_.push_back(std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(io_context&));
    }
}

auto packet_dispatcher::dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void
{
    ASSERT_RETURN(handlers_.size() > packet_id);

    if (packet_id >= handlers_.size()) {
        return;
    }

    const auto& handler = handlers_[packet_id];
    if (!handler) {
        // todo : unsupport packet_id
        return;
    }

    if (strands_.empty()) {
        handler(sess, body_data, body_size);
        return;
    }

    uint64_t sess_id = sess->get_index();
    size_t strand_idx = sess_id % strands_.size();

    // 비동기 포스팅을 위한 바디 데이터 복사
    auto body_copy = std::make_shared<std::vector<uint8_t>>(body_data, body_data + body_size);
    
    // post로 처리
    boost::asio::post(*strands_[strand_idx], [handler, sess, body_copy]() {
        handler(sess, body_copy->data(), body_copy->size());
        });
}