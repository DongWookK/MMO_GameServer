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

	auto it_default = maps_.find(default_map_no);
	if (it_default == maps_.end() || it_default->second->get_type() != common::map_type::permanent)
	{
		FLOG_CRITICAL("map_manager :: default map({}) not exist or not permanent", default_map_no);
		return error::code::map_load_fail;
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

auto map_manager::enter_map(const game_map::gobject_s_ptr_t& object, map_no_t map_no, const vec3& pos, float heading) -> fw::error
{
	ASSERT_RETURN_VALUE(object != nullptr, error::code::object_spawn_fail);

	game_map* map = find_map(map_no);
	if (map != nullptr && map->get_type() == common::map_type::permanent)
	{
		return map->enter(object, pos, heading);
	}

	game_map* default_map = find_map(default_map_no);
	ASSERT_RETURN_VALUE(default_map != nullptr, error::code::map_not_exist);

	if (map_no != gobject::invalid_map_id)
	{
		FLOG_WARN("map_manager :: object({}) map({}) can not enter -> default map({})", object->get_object_id(), map_no, default_map_no);
	}

	return default_map->enter(object, default_map->get_random_pc_spawn_pos(), heading);
}

auto map_manager::exit_map(const game_map::gobject_s_ptr_t& object) -> fw::error
{
	ASSERT_RETURN_VALUE(object != nullptr, error::code::object_despawn_fail);
	ASSERT_RETURN_VALUE(object->is_spawned(), error::code::object_despawn_fail);

	game_map* map = find_map(object->get_map_id());
	ASSERT_RETURN_VALUE(map != nullptr, error::code::map_not_exist);

	return map->exit(object);
}

auto map_manager::get_nav_dir() -> std::filesystem::path
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::filesystem::path(buffer).parent_path() / nav_dir_name;
}
