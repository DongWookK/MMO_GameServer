#pragma once
#include "pch.h"

class session;
struct packet_header;

template <typename T>
using packet_handler_t = std::function<void(const std::shared_ptr<session>&, const T*)>;

// 내부 저장을 위한 로우 바이트 핸들러 타입 (기존과 동일)
using raw_handler_t = std::function<void(const std::shared_ptr<session>&, const uint8_t*, size_t)>;

class packet_dispatcher : public singleton<packet_dispatcher>
{
public:
    template <typename T>
    using packet_handler_t = std::function<void(const std::shared_ptr<session>&, const T*)>;
    using raw_handler_t = std::function<void(const std::shared_ptr<session>&, const uint8_t*, size_t)>;
    using strands_s_ptr_t = std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>>;

public:
    template <typename T>
    auto register_handler(uint16_t packet_id, packet_handler_t<T> handler) -> void
    {
        if (handlers_.size() <= packet_id) {
            handlers_.resize(packet_id + 1);
        }

        // 바이트 데이터를 FlatBuffer 객체로 변환하는 람다 함수로 감싸서 저장
        handlers_[packet_id] = [handler = std::move(handler)](const std::shared_ptr<session>& sess,
            const uint8_t* body_data,
            size_t body_size)
            {
                if (!body_data || body_size == 0) return;

                flatbuffers::Verifier verifier(body_data, body_size);
                if (!verifier.VerifyBuffer<T>(nullptr)) {
                    ASSERT_RETURN(false);
                    return;
                }

                const T* packet_obj = flatbuffers::GetRoot<T>(body_data);
                handler(sess, packet_obj);
            };

        FLOG_INFO("packet({}) Handler registred");
    }

    auto set_strands(boost::asio::io_context* io_context, size_t strand_count) -> void;
    auto dispatch(const std::shared_ptr<session>& sess, uint16_t packet_id, const uint8_t* body_data, size_t body_size) -> void;

private:
    std::vector<strands_s_ptr_t> strands_{};
    std::vector<raw_handler_t> handlers_;
};