#pragma once
#include "pch.h"
#include "gobject.h"
#include "nav_mesh.h"

class game_map
{
public:
	using map_no_t = gobject::map_no_t;

public:
	game_map(map_no_t map_id, std::string_view name);

	game_map(const game_map&) = delete;
	game_map& operator=(const game_map&) = delete;

public:
	// nav_dir / <name>.navmesh 로드
	auto load(const std::filesystem::path& nav_dir) -> fw::error;

	auto get_map_id() const -> map_no_t { return map_id_; }
	auto get_name() const -> const std::string& { return name_; }
	auto get_nav() const -> const nav_mesh& { return nav_; }

private:
	const map_no_t map_id_;
	const std::string name_;
	nav_mesh nav_;
};
