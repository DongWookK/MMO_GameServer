#pragma once
#include "pch.h"
#include "game_generated.h"
#include "enum_game_generated.h"

class troc_user
{
    void declare_tr_user_login_req()
    {
        packet_dispatcher::instance()->register_handler<game::TestEcho>(
            std::to_underlying(game::tr_type::TestEcho),
            [this](const std::shared_ptr<session>& sess, const game::TestEcho* pkt) {
                this->handler_user_login_req(sess, pkt);
            }
        );
    }

private:
    auto handler_user_login_req(const std::shared_ptr<session>& sess, const game::TestEcho* pkt) -> void;

};