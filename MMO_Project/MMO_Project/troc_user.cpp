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

    flatbuffers::FlatBufferBuilder builder;

    // Create 함수 호출 시에도 PacketTraits를 활용해 tr_type 자동 주입
    auto offset = game::CreateTestEcho(
        builder,
        std::to_underlying(PacketTraits<game::TestEcho>::type),
        builder.CreateString(pkt->data())
    );

    // [개선] Finish 및 tr_type 추출 및 send를 자동 처리!
    sess->send_packet(builder, offset);

    return error;
}

HANDLER_TR_DEFINE(troc_user, UserLoginReq)
{
    fw::error error{};

    if (!sess) {
        return fw::error{ 111 };
    }

    flatbuffers::FlatBufferBuilder builder;
    // LoginReq 관련 패킷 생성...
    // auto offset = game::CreateUserLoginReq(...);
    // send_packet(sess, builder, offset);

    return error;
}