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

    auto offset = game::CreateTestEcho(builder,std::to_underlying(PacketTraits<game::TestEcho>::type),builder.CreateString(pkt->data()));
    sess->send_packet(builder, offset);

    return error;
}

HANDLER_TR_DEFINE(troc_user, UserLoginReq)
{
    fw::error error{};

    if (!sess) {
        return fw::error{ 111 };
    }

    auto user = user_manager::instance()->user_login(sess);
    ASSERT_RETURN_VALUE(nullptr != user, error::code::user_login_fail);

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreateUserLoginAck(builder, std::to_underlying(PacketTraits<game::UserLoginAck>::type), user->get_user_no());
    sess->send_packet(builder, offset);

    return error;
}

HANDLER_TR_DEFINE(troc_user, UserLogoutReq)
{
    fw::error error{};

    if (!sess) {
        return fw::error{ 111 };
    }

    error = user_manager::instance()->user_logout(sess);
    ASSERT_RETURN_VALUE(!(error), error::code::user_login_fail);

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreateUserLoginAck(builder, std::to_underlying(PacketTraits<game::UserLogoutAck>::type), 123);
    sess->send_packet(builder, offset);

    return error;
}