#pragma once
#include "pch.h"
#include <type_traits>
#include <flatbuffers/flatbuffers.h>

// 1. 기본 PacketTraits 템플릿 선언
template <typename T>
struct PacketTraits;

// 2. 패킷별 tr_type 1:1 매핑 매크로 (세미콜론 포함)
#define DECLARE_PACKET_TRAITS(Type, PacketName) \
    template <> \
    struct PacketTraits<game::PacketName> { \
        static constexpr game::tr_type type = Type; \
    };

// 3. 핸들러 선언 매크로
#define HANDLER_TR(PacketName) \
    fw::error handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt);

// 4. 핸들러 구현 매크로
#define HANDLER_TR_DEFINE(ClassName, PacketName) \
    fw::error ClassName::handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt)

// 5. 디스패처 패킷 등록 매크로
#define REGISTER_TR(Type, PacketName) \
    [this]() { \
        packet_dispatcher::instance()->register_handler<game::PacketName>( \
            std::to_underlying(Type), \
            [this](const std::shared_ptr<session>& sess, const game::PacketName* pkt) { \
                this->handler_tr_##PacketName(sess, pkt); \
            } \
        ); \
    }();