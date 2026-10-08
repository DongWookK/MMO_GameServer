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

    send_pc_list_notify(sess);

    return error_code;
}

auto troc_user::send_pc_list_notify(const std::shared_ptr<session>& sess) -> void
{
    auto list_result = user_manager::instance()->pc_list(sess);
    if (!list_result.has_value())
    {
        // 로그인 자체는 성공했으므로 Nak 대신 로그만 남긴다 (클라이언트는 목록 없이 캐릭터 선택 화면)
        FLOG_ERROR("send_pc_list_notify :: pc_list fail session({}) error({})", sess->get_index(), list_result.error().value());
        return;
    }

    flatbuffers::FlatBufferBuilder builder;

    std::vector<flatbuffers::Offset<game::PcSummary>> summaries{};
    summaries.reserve(list_result->size());
    for (const auto& summary : *list_result)
    {
        summaries.push_back(game::CreatePcSummary(builder
                                                  , summary.pc_no
                                                  , fw::create_string(builder, summary.pc_name)
                                                  , std::to_underlying(summary.pc_type)
                                                  , summary.level));
    }

    auto offset = game::CreatePcListNotifyDirect(builder
                                                 , std::to_underlying(PacketTraits<game::PcListNotify>::type)
                                                 , &summaries);
    sess->send_packet(builder, offset);
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

HANDLER_TR_DEFINE(troc_user, PcCreateReq)
{
    ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);

    fw::error error_code{};
    DEFER_NAK(PcCreateReq, error_code);

    const auto pc_name = fw::to_wstring(pkt->pc_name());
    const auto pc_type = static_cast<common::pc_type>(pkt->pc_type());

    auto create_result = user_manager::instance()->pc_create(sess, pc_name, pc_type);
    ASSERT_RETURN_ERROR(create_result.has_value(), error_code, create_result.error());

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreatePcCreateAck(builder
                                          , std::to_underlying(PacketTraits<game::PcCreateAck>::type)
                                          , *create_result
                                          , fw::create_string(builder, pc_name)
                                          , std::to_underlying(pc_type));
    sess->send_packet(builder, offset);

    return error_code;
}

HANDLER_TR_DEFINE(troc_user, PcSelectReq)
{
    ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);

    fw::error error_code{};
    DEFER_NAK(PcSelectReq, error_code);

    auto select_result = user_manager::instance()->pc_select(sess, pkt->pc_no());
    ASSERT_RETURN_ERROR(select_result.has_value(), error_code, select_result.error());

    const auto& pc = *select_result;
    const auto& pos = pc->get_pos();
    const game::Vec3 pkt_pos{ pos.x, pos.y, pos.z };

    flatbuffers::FlatBufferBuilder builder;
    auto offset = game::CreatePcSelectAck(builder
                                          , std::to_underlying(PacketTraits<game::PcSelectAck>::type)
                                          , pc->get_object_id().value
                                          , pc->get_pc_no()
                                          , fw::create_string(builder, pc->get_name())
                                          , std::to_underlying(pc->get_pc_type())
                                          , pc->get_level()
                                          , pc->get_exp()
                                          , pc->get_hp()
                                          , pc->get_mp()
                                          , &pkt_pos
                                          , pc->get_map_id());
    sess->send_packet(builder, offset);

    return error_code;
}