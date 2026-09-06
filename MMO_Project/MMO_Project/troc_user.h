#pragma once
#include "pch.h"
#include "game_generated.h"
#include "enum_game_generated.h"
#include "packet_dispatcher.h"

// game::TestEcho 타입이 정의된 헤더(game_generated.h) 뒤에 배치
DECLARE_PACKET_TRAITS(game::tr_type::TestEcho, TestEcho);
DECLARE_PACKET_TRAITS(game::tr_type::UserLoginReq, UserLoginReq);
DECLARE_PACKET_TRAITS(game::tr_type::UserLoginAck, UserLoginAck);

class troc_user : public feature
{
public:
    auto setup() -> fw::error override
    {
        [this]() { packet_dispatcher::instance()->register_handler<game::TestEcho>(std::to_underlying(game::tr_type::TestEcho), [this](const std::shared_ptr<session>& sess, const game::TestEcho* pkt) { this->handler_tr_TestEcho(sess, pkt); }); }();;
        [this]() { packet_dispatcher::instance()->register_handler<game::UserLoginReq>(std::to_underlying(game::tr_type::UserLoginReq), [this](const std::shared_ptr<session>& sess, const game::UserLoginReq* pkt) { this->handler_tr_UserLoginReq(sess, pkt); }); }();;

        return fw::error{};
    }

public:
    HANDLER_TR(TestEcho);
    HANDLER_TR(UserLoginReq);
};