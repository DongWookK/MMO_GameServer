#pragma once
#include "pch.h"
#include "map_manager.h"
#include "map_info_manager.h"

auto map_manager::setup() -> fw::error
{
	const auto nav_dir = get_nav_dir();

	std::unique_lock lock(lock_);

	for (const auto& [map_no, info] : map_info_manager::instance()->get_map_infos())
	{
		auto map = std::make_unique<game_map>(info);

		auto error = map->load(nav_dir);
		if (error)
		{
			FLOG_CRITICAL("map_manager :: map({}:{}) load failed. nav_dir({}) filename({})", map_no, fw::wstring_to_string(info.name), nav_dir.string(), fw::wstring_to_string(info.filename));
			return error;
		}

		maps_.emplace(map_no, std::move(map));
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

auto map_manager::find_map(map_no_t map_no) const -> game_map*
{
	std::shared_lock lock(lock_);

	auto it = maps_.find(map_no);
	return it != maps_.end() ? it->second.get() : nullptr;
}

auto map_manager::get_nav_dir() -> std::filesystem::path
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path() / nav_dir_name;
}
