#pragma once
#include "pch.h"
#include "troc_user.h"
#include "session.h"
#include "user_manager.h"

HANDLER_TR_DEFINE(troc_user, TestEcho)
{
    fw::error error{};

    if (!sess) {
        return fw::error{ 111 };
    }

    // 핸들러 로직
    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreateTestEcho(builder, std::to_underlying(game::tr_type::TestEcho), builder.CreateString(pkt->data()));
    builder.Finish(offset);

    sess->send(std::to_underlying(game::tr_type::TestEcho), builder);
    
    return error;
}

HANDLER_TR_DEFINE(troc_user, LoginReq)
{
    fw::error error{};
    
    if (!sess) {
        return fw::error{ 111 };
    }

    //sess->send()
    

    return error;
}
 