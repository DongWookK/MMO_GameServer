#pragma once
#include "pch.h"
#include "game_map.h"

game_map::game_map(map_id_t map_id, std::string_view name)
	: map_id_(map_id)
	, name_(name)
{
}

auto game_map::load(const std::filesystem::path& nav_dir) -> fw::error
{
	const auto nav_path = nav_dir / (name_ + ".navmesh");

	auto error = nav_.load(nav_path);
	ASSERT_RETURN_VALUE(!error, error);

	FLOG_INFO("game_map :: loaded map({}:{}) nav tiles({}) polys({})", map_id_, name_, nav_.get_tile_count(), nav_.get_poly_count());
	return error;
}
