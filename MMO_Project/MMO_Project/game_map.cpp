#pragma once
#include "pch.h"
#include "game_map.h"
#include <random>

game_map::game_map(const map_info& info)
	: info_(info)
{
}

auto game_map::load(const std::filesystem::path& nav_dir) -> fw::error
{
	const map_info& info = info_.get();
	const auto nav_path = nav_dir / (info.filename + L".navmesh");

	auto error = nav_.load(nav_path);
	ASSERT_RETURN_VALUE(!error, error);

	for (const map_point& point : info.points)
	{
		if (point.type != common::map_point_type::pc_spawn)
		{
			continue;
		}

		vec3 nav_pos{};
		if (!nav_.find_nearest_point(point.pos, nav_pos))
		{
			FLOG_CRITICAL("game_map :: map({}) point({}) pos({}, {}, {}) is not on navmesh", info.map_no, point.point_no, point.pos.x, point.pos.y, point.pos.z);
			return error::code::map_load_fail;
		}

		pc_spawn_positions_.push_back(nav_pos);
	}

	if (pc_spawn_positions_.empty())
	{
		FLOG_CRITICAL("game_map :: map({}:{}) has no pc_spawn point in dt_map_point", info.map_no, fw::wstring_to_string(info.name));
		return error::code::map_load_fail;
	}

	FLOG_INFO("game_map :: loaded map({}:{}) type({}) nav({}) tiles({}) polys({}) pc_spawn({})", info.map_no, fw::wstring_to_string(info.name),
		common::EnumNamemap_type(info.type), fw::wstring_to_string(info.filename), nav_.get_tile_count(), nav_.get_poly_count(), pc_spawn_positions_.size());
	return error;
}

auto game_map::enter(const gobject_s_ptr_t& object, const vec3& pos, float heading) -> fw::error
{
	ASSERT_RETURN_VALUE(object != nullptr, error::code::object_spawn_fail);

	// 저장 위치가 내비메시 밖이면 (맵 수정, 잘못된 좌표) 스폰 포인트로 보낸다
	vec3 spawn_pos{};
	if (!nav_.find_nearest_point(pos, spawn_pos))
	{
		spawn_pos = get_random_pc_spawn_pos();
		FLOG_WARN("game_map :: map({}) object({}) pos({}, {}, {}) is not on navmesh -> spawn point({}, {}, {})", get_map_no(), object->get_object_id().value,
			pos.x, pos.y, pos.z, spawn_pos.x, spawn_pos.y, spawn_pos.z);
	}

	std::unique_lock lock(lock_);

	ASSERT_RETURN_VALUE(!objects_.contains(object->get_object_id()), error::code::object_spawn_fail);

	auto error = object->spawn(get_map_no(), spawn_pos, heading);
	ASSERT_RETURN_VALUE(!error, error);

	objects_.emplace(object->get_object_id(), object);
	return error;
}

auto game_map::exit(const gobject_s_ptr_t& object) -> fw::error
{
	ASSERT_RETURN_VALUE(object != nullptr, error::code::object_despawn_fail);

	std::unique_lock lock(lock_);

	auto it = objects_.find(object->get_object_id());
	ASSERT_RETURN_VALUE(it != objects_.end() && it->second == object, error::code::object_despawn_fail);

	objects_.erase(it);
	return object->despawn();
}

auto game_map::find_object(object_id_t object_id) const -> gobject_s_ptr_t
{
	std::shared_lock lock(lock_);

	auto it = objects_.find(object_id);
	return it != objects_.end() ? it->second : nullptr;
}

auto game_map::get_object_count() const -> size_t
{
	std::shared_lock lock(lock_);
	return objects_.size();
}

auto game_map::get_random_pc_spawn_pos() const -> const vec3&
{
	thread_local std::mt19937 engine{ std::random_device{}() };
	std::uniform_int_distribution<size_t> dist(0, pc_spawn_positions_.size() - 1);
	return pc_spawn_positions_[dist(engine)];
}
