#pragma once
#include "pch.h"
#include "gobject.h"

gobject::gobject(common::object_type type)
	: object_id_(generate_object_id())
	, object_type_(type)
{
}

auto gobject::spawn(map_id_t map_id, const vec3& pos, float heading) -> fw::error
{
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

auto gobject::generate_object_id() -> object_id_t
{
	static std::atomic<object_id_t> next_id{ 1 };
	return next_id.fetch_add(1, std::memory_order_relaxed);
}
