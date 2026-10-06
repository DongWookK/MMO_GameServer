#pragma once
#include "pch.h"
#include "map_manager.h"

auto map_manager::setup() -> fw::error
{
	const auto nav_dir = get_nav_dir();

	std::unique_lock lock(lock_);

	for (const map_define& define : map_defines)
	{
		auto map = std::make_unique<game_map>(define.map_id, define.name);

		auto error = map->load(nav_dir);
		if (error)
		{
			FLOG_CRITICAL("map_manager :: map({}:{}) load failed. nav_dir({})", define.map_id, define.name, nav_dir.string());
			return error;
		}

		maps_.emplace(define.map_id, std::move(map));
	}

	FLOG_INFO("map_manager :: {} map(s) loaded from {}", maps_.size(), nav_dir.string());
	return fw::error{};
}

auto map_manager::start() -> fw::error
{
	return fw::error{};
}

auto map_manager::stop() -> fw::error
{
	return fw::error{};
}

auto map_manager::teardown() -> fw::error
{
	std::unique_lock lock(lock_);
	maps_.clear();
	return fw::error{};
}

auto map_manager::find_map(map_no_t map_id) const -> game_map*
{
	std::shared_lock lock(lock_);

	auto it = maps_.find(map_id);
	return it != maps_.end() ? it->second.get() : nullptr;
}

auto map_manager::get_nav_dir() -> std::filesystem::path
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path() / nav_dir_name;
}
