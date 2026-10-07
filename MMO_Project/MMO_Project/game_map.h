#pragma once
#include "pch.h"
#include "map_info_manager.h"
#include "nav_mesh.h"

class game_map
{
public:
	using map_no_t = map_info::map_no_t;

public:
	explicit game_map(const map_info& info);

	game_map(const game_map&) = delete;
	game_map& operator=(const game_map&) = delete;

public:
	// nav_dir / <filename>.navmesh 로드
	auto load(const std::filesystem::path& nav_dir) -> fw::error;

	auto get_info() const -> const map_info& { return info_.get(); }
	auto get_map_no() const -> map_no_t { return info_.get().map_no; }
	auto get_type() const -> common::map_type { return info_.get().type; }
	auto get_name() const -> const std::wstring& { return info_.get().name; }
	auto get_nav() const -> const nav_mesh& { return nav_; }

private:
	std::reference_wrapper<const map_info> info_;
	nav_mesh nav_;
};
