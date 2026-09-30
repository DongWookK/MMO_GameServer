#pragma once
#include "pch.h"
#include <type_traits>
#include <flatbuffers/flatbuffers.h>

template <typename T>
struct PacketTraits;

#define DECLARE_PACKET_TRAITS(Type, PacketName) \
    template <> \
    struct PacketTraits<game::PacketName> { \
        static constexpr game::tr_type type = Type; \
    };

#define HANDLER_TR(PacketName) \
    fw::error handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt);

#define HANDLER_TR_DEFINE(ClassName, PacketName) \
    fw::error ClassName::handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt)

#define REGISTER_TR(Type, PacketName) \
    [this]() { \
        packet_dispatcher::instance()->register_handler<game::PacketName>( \
            std::to_underlying(Type), \
            [this](const std::shared_ptr<session>& sess, const game::PacketName* pkt) { \
                this->handler_tr_##PacketName(sess, pkt); \
            } \
        ); \
    }();

#define DEFER_NAK(PacketName, error_var)                                                      \
    DEFER(                                                                                    \
        if (error_var) {                                                                      \
            flatbuffers::FlatBufferBuilder nak_builder;                                       \
            auto nak_offset = game::CreateNak(nak_builder,                                    \
                std::to_underlying(PacketTraits<game::Nak>::type),                            \
                std::to_underlying(PacketTraits<game::PacketName>::type),                     \
                static_cast<uint16_t>(error_var));                                            \
            sess->send_packet(nak_builder, nak_offset);                                       \
        }                                                                                     \
    )