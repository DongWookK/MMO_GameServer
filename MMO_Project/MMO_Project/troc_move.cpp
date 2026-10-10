#pragma once
#include "pch.h"
#include "troc_move.h"
#include "session.h"
#include "user_manager.h"
#include "map_manager.h"

HANDLER_TR_DEFINE(troc_move, MoveReq)
{
	ASSERT_RETURN_VALUE(sess != nullptr, error::code::session_invalid);

	fw::error error_code{};
	DEFER_NAK(MoveReq, error_code);

	ASSERT_RETURN_ERROR(pkt->dest() != nullptr, error_code, error::code::move_not_allowed);

	auto pc = user_manager::instance()->find_pc_by_session(sess);
	ASSERT_RETURN_ERROR(pc != nullptr, error_code, error::code::pc_not_exist);
	ASSERT_RETURN_ERROR(pc->is_spawned(), error_code, error::code::move_not_allowed);

	const auto now = character::clock_t::now();
	if (now - pc->get_last_move_request() < min_move_interval)
	{
		return error_code;
	}
	pc->set_last_move_request(now);

	auto* map = map_manager::instance()->find_map(pc->get_map_id());
	ASSERT_RETURN_ERROR(map != nullptr, error_code, error::code::map_not_exist);

	const vec3 start = pc->update_move(now);
	const vec3 dest{ pkt->dest()->x(), pkt->dest()->y(), pkt->dest()->z() };

	std::vector<vec3> path{};
	bool partial = false;
	if (!map->get_nav().find_path(start, dest, path, &partial))
	{
		FLOG_WARN("MoveReq :: path fail object({}) start({}, {}, {}) dest({}, {}, {})", pc->get_object_id().value,
			start.x, start.y, start.z, dest.x, dest.y, dest.z);
		error_code = error::code::move_path_fail;
		return error_code;
	}

	if (path.size() < 2) // 도착
	{
		pc->stop_move(now);
		path.assign(1, pc->get_pos());
	}
	else
	{
		pc->start_move(path, now);
	}

	std::vector<game::Vec3> pkt_path{};
	pkt_path.reserve(path.size());
	for (const vec3& point : path)
	{
		pkt_path.emplace_back(point.x, point.y, point.z);
	}

	flatbuffers::FlatBufferBuilder builder;
	auto offset = game::CreateMoveNotify(builder
										 , std::to_underlying(PacketTraits<game::MoveNotify>::type)
										 , pc->get_object_id().value
										 , builder.CreateVectorOfStructs(pkt_path)
										 , pc->get_move_speed());
	// todo : 섹터 도입 후 주변 섹터로 브로드캐스트. 지금은 요청자에게만
	sess->send_packet(builder, offset);

	return error_code;
}
