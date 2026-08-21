#pragma once
#include "pch.h"
#include "packet_dispatcher.h"
#include "session.h"

auto packet_dispatcher::set_strands(boost::asio::io_context* io_context, size_t strand_count) -> void
{
    for (uint32_t i = 0; i < strand_count; ++i)
    {
        strands_.push_back(std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(io_context->get_executor()));
    }

    FLOG_INFO("packet_dispatcher:: setup strand_count({})", strands_.size());
}

auto packet_dispatcher::dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void
{
    ASSERT_RETURN(handlers_.size() > packet_id);
    ASSERT_RETURN(!strands_.empty());

    if (packet_id >= handlers_.size()) {
        return;
    }

    const auto& handler = handlers_[packet_id];
    if (!handler) {
        FLOG_ERROR("UNSUPPORT PACKET entered packet_id({})", packet_id);
        ASSERT_RETURN(false);
        return;
    }

    uint64_t sess_id = sess->get_index();
    size_t strand_idx = sess_id % strands_.size();
    // 디버그로 strnad_idx 제대로 들어오는지 찍어보기

    // 비동기 포스팅을 위한 바디 데이터 복사
    auto body_copy = std::make_shared<std::vector<uint8_t>>(body_data, body_data + body_size);
    
    boost::asio::post(*strands_[strand_idx], [handler, sess, body_copy]() {
        handler(sess, body_copy->data(), body_copy->size());
        });
}