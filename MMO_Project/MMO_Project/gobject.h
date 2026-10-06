#pragma once
#include "pch.h"
#include "enum_common_generated.h"
#include "vec3.h"

class gobject : public std::enable_shared_from_this<gobject>
{
public:
	using object_id_t = uint64_t;
	using map_no_t = uint32_t;
	using sector_id_t = uint32_t;

	static constexpr object_id_t invalid_object_id = 0;
	static constexpr map_no_t invalid_map_id = 0;
	static constexpr sector_id_t invalid_sector_id = (std::numeric_limits<sector_id_t>::max)();

public:
	explicit gobject(common::object_type type);
	virtual ~gobject() = default;

	gobject(const gobject&) = delete;
	gobject& operator=(const gobject&) = delete;
	gobject(gobject&&) = delete;
	gobject& operator=(gobject&&) = delete;

public:
	auto spawn(map_no_t map_id, const vec3& pos, float heading) -> fw::error;
	auto despawn() -> fw::error;

	virtual auto is_movable() const -> bool { return false; }

public:
	auto get_object_id() const -> object_id_t { return object_id_; }
	auto get_object_type() const -> common::object_type { return object_type_; }
	auto is_spawned() const -> bool { return spawned_; }

	auto get_map_id() const -> map_no_t { return map_id_; }
	auto get_pos() const -> const vec3& { return pos_; }
	auto get_heading() const -> float { return heading_; }
	auto get_sector_id() const -> sector_id_t { return sector_id_; }

	auto set_pos(const vec3& pos) -> void { pos_ = pos; }
	auto set_heading(float heading) -> void { heading_ = heading; }
	auto set_sector_id(sector_id_t sector_id) -> void { sector_id_ = sector_id; }

protected:
	virtual auto on_spawn() -> void {}
	virtual auto on_despawn() -> void {}

private:
	static auto generate_object_id() -> object_id_t;

private:
	const object_id_t       object_id_;
	const common::object_type object_type_;

	bool        spawned_ = false;
	map_no_t    map_id_ = invalid_map_id;
	vec3        pos_{};
	float       heading_ = 0.f;
	sector_id_t sector_id_ = invalid_sector_id;
};
