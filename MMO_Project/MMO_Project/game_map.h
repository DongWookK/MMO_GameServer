#pragma once
#include "pch.h"
#include <shared_mutex>
#include "map_info_manager.h"
#include "nav_mesh.h"

class game_map
{
public:
	using map_no_t = map_info::map_no_t;
	using object_id_t = gobject::object_id_t;
	using gobject_s_ptr_t = std::shared_ptr<gobject>;

public:
	explicit game_map(const map_info& info);

	game_map(const game_map&) = delete;
	game_map& operator=(const game_map&) = delete;

public:
	// nav_dir / <filename>.navmesh 로드
	auto load(const std::filesystem::path& nav_dir) -> fw::error;

	auto enter(const gobject_s_ptr_t& object, const vec3& pos, float heading) -> fw::error;
	auto exit(const gobject_s_ptr_t& object) -> fw::error;

	auto find_object(object_id_t object_id) const -> gobject_s_ptr_t;
	auto get_object_count() const -> size_t;

	auto get_random_pc_spawn_pos() const -> const vec3&;

	auto get_info() const -> const map_info& { return info_.get(); }
	auto get_map_no() const -> map_no_t { return info_.get().map_no; }
	auto get_type() const -> common::map_type { return info_.get().type; }
	auto get_name() const -> const std::wstring& { return info_.get().name; }
	auto get_nav() const -> const nav_mesh& { return nav_; }

private:
	std::reference_wrapper<const map_info> info_;
	nav_mesh nav_;
	std::vector<vec3> pc_spawn_positions_{};

	mutable std::shared_mutex lock_{};
	std::unordered_map<object_id_t, gobject_s_ptr_t> objects_{};
};
