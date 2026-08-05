#pragma once

#define REGISTER_TR(Type, PacketName) \
    [this]() { \
        packet_dispatcher::instance()->register_handler<game::PacketName>( \
            std::to_underlying(Type), \
            [this](const std::shared_ptr<session>& sess, const game::PacketName* pkt) { \
                this->handler_tr_##PacketName(sess, pkt); \
            } \
        ); \
    }();

#define HANDLER_TR(PacketName) \
    fw::error handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt);

#define HANDLER_TR_DEFINE(ClassName, PacketName) \
    fw::error ClassName::handler_tr_##PacketName(const std::shared_ptr<session>& sess, const game::PacketName* pkt)