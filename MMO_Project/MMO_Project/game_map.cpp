#pragma once
#include "pch.h"
#include "game_map.h"

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

	FLOG_INFO("game_map :: loaded map({}:{}) type({}) nav({}) tiles({}) polys({})", info.map_no, fw::wstring_to_string(info.name),
		common::EnumNamemap_type(info.type), fw::wstring_to_string(info.filename), nav_.get_tile_count(), nav_.get_poly_count());
	return error;
}
