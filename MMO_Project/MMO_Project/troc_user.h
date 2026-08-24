#pragma once
#include "pch.h"
#include "game_generated.h"
#include "enum_game_generated.h"
#include "packet_dispatcher.h"

class troc_user : public feature
{
public:
    auto setup() -> fw::error override
    {
        REGISTER_TR(game::tr_type::TestEcho, TestEcho);
        REGISTER_TR(game::tr_type::UserLoginReq, LoginReq);

        return fw::error{};
    }

public:
    HANDLER_TR(TestEcho);
    HANDLER_TR(LoginReq);
};