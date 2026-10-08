#pragma once
#include "pch.h"
#include "gobject.h"

gobject::gobject(common::object_type type)
	: object_type_(type)
{
}

auto gobject::set_pool_index(pool_index_t index) -> void
{
	ASSERT_CRASH(index <= object_id_t::max_index);

	object_id_ = object_id_t{ object_type_, 0, index };
}

auto gobject::on_release() -> void
{
	spawned_ = false;
	map_id_ = invalid_map_id;
	pos_ = vec3{};
	heading_ = 0.f;
	sector_id_ = invalid_sector_id;

	object_id_ = object_id_t{ object_type_, object_id_.reuse_count + 1u, object_id_.index };	// reuse_count 8bit 순환
}

auto gobject::spawn(map_no_t map_id, const vec3& pos, float heading) -> fw::error
{
	ASSERT_RETURN_VALUE(object_id_.is_valid(), error::code::object_spawn_fail);
	ASSERT_RETURN_VALUE(!spawned_, error::code::object_spawn_fail);
	ASSERT_RETURN_VALUE(map_id != invalid_map_id, error::code::object_spawn_fail);

	map_id_ = map_id;
	pos_ = pos;
	heading_ = heading;
	spawned_ = true;

	on_spawn();

	return fw::error{};
}

auto gobject::despawn() -> fw::error
{
	ASSERT_RETURN_VALUE(spawned_, error::code::object_despawn_fail);

	on_despawn();

	spawned_ = false;
	map_id_ = invalid_map_id;
	sector_id_ = invalid_sector_id;

	return fw::error{};
}
