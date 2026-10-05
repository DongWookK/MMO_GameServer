#pragma once
#include "pch.h"
#include "game_generated.h"
#include "enum_game_generated.h"
#include "packet_dispatcher.h"

// game::TestEcho 타입이 정의된 헤더(game_generated.h) 뒤에 배치
DECLARE_PACKET_TRAITS(game::tr_type::TestEcho, TestEcho);
DECLARE_PACKET_TRAITS(game::tr_type::UserLoginReq, UserLoginReq);
DECLARE_PACKET_TRAITS(game::tr_type::UserLoginAck, UserLoginAck);
DECLARE_PACKET_TRAITS(game::tr_type::UserLogoutReq, UserLogoutReq);
DECLARE_PACKET_TRAITS(game::tr_type::UserLogoutAck, UserLogoutAck);
DECLARE_PACKET_TRAITS(game::tr_type::Nak, Nak);
DECLARE_PACKET_TRAITS(game::tr_type::PcCreateReq, PcCreateReq);
DECLARE_PACKET_TRAITS(game::tr_type::PcCreateAck, PcCreateAck);
DECLARE_PACKET_TRAITS(game::tr_type::PcSelectReq, PcSelectReq);
DECLARE_PACKET_TRAITS(game::tr_type::PcSelectAck, PcSelectAck);
DECLARE_PACKET_TRAITS(game::tr_type::PcListNotify, PcListNotify);

class troc_user : public feature, singleton<troc_user>
{
public:
    troc_user() : feature("troc_user") {}

public:
    auto setup() -> fw::error override
    {
        [this]() { packet_dispatcher::instance()->register_handler<game::TestEcho>(std::to_underlying(game::tr_type::TestEcho), [this](const std::shared_ptr<session>& sess, const game::TestEcho* pkt) { this->handler_tr_TestEcho(sess, pkt); }); }();;
        [this]() { packet_dispatcher::instance()->register_handler<game::UserLoginReq>(std::to_underlying(game::tr_type::UserLoginReq), [this](const std::shared_ptr<session>& sess, const game::UserLoginReq* pkt) { this->handler_tr_UserLoginReq(sess, pkt); }); }();;
        [this]() { packet_dispatcher::instance()->register_handler<game::UserLogoutReq>(std::to_underlying(game::tr_type::UserLogoutReq), [this](const std::shared_ptr<session>& sess, const game::UserLogoutReq* pkt) { this->handler_tr_UserLogoutReq(sess, pkt); }); }();;
        REGISTER_TR(game::tr_type::PcCreateReq, PcCreateReq);
        REGISTER_TR(game::tr_type::PcSelectReq, PcSelectReq);

        return fw::error{};
    }

public:
    HANDLER_TR(TestEcho);
    HANDLER_TR(UserLoginReq);
    HANDLER_TR(UserLogoutReq);
    HANDLER_TR(PcCreateReq);
    HANDLER_TR(PcSelectReq);

private:
    auto send_pc_list_notify(const std::shared_ptr<session>& sess) -> void;
};