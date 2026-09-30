#pragma once
#include "pch.h"
#include "troc_user.h"
#include "session.h"
#include "user_manager.h"

HANDLER_TR_DEFINE(troc_user, TestEcho)
{
    ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);
    fw::error error_code{};

    flatbuffers::FlatBufferBuilder builder;

    auto offset = game::CreateTestEcho(builder
                                       , std::to_underlying(PacketTraits<game::TestEcho>::type)
                                       , builder.CreateString(pkt->data()));
    sess->send_packet(builder, offset);

    return error_code;
}

HANDLER_TR_DEFINE(troc_user, UserLoginReq)
{
    ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);

    fw::error error_code{};
    DEFER_NAK(UserLoginReq, error_code);

    const auto user_name = fw::to_wstring(pkt->user_name());
    auto login_result = user_manager::instance()->user_login(sess, user_name);
    ASSERT_RETURN_ERROR(login_result.has_value(), error_code, login_result.error());

    const auto& user = *login_result;

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreateUserLoginAck(builder
                                           , std::to_underlying(PacketTraits<game::UserLoginAck>::type)
                                           , fw::create_string(builder, user_name)
                                           , user->get_user_no());
    sess->send_packet(builder, offset);

    return error_code;
}

HANDLER_TR_DEFINE(troc_user, UserLogoutReq)
{
    ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);

    fw::error error_code{};
    DEFER_NAK(UserLogoutReq, error_code);

    error_code = user_manager::instance()->user_logout(sess);
    ASSERT_RETURN_VALUE(!(error_code), error_code);

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreateUserLogoutAck(builder
                                            , std::to_underlying(PacketTraits<game::UserLogoutAck>::type)
                                            , 0);
    sess->send_packet(builder, offset);

    return error_code;
}
