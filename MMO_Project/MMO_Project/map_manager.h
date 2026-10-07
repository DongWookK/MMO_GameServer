#pragma once
#include "pch.h"
#include <shared_mutex>
#include "game_map.h"

class map_manager : public feature, public singleton<map_manager>
{
	friend class singleton<map_manager>;

public:
	using map_no_t = game_map::map_no_t;

	static constexpr std::wstring_view nav_dir_name = L"nav";

	static constexpr map_no_t default_map_no = 1;

public:
	map_manager()
		: feature("map_manager")
	{}
	~map_manager() override = default;

public:
	auto setup() -> fw::error override;
	auto start() -> fw::error override;
	auto stop() -> fw::error override;
	auto teardown() -> fw::error override;

public:
	auto find_map(map_no_t map_no) const -> game_map*;

	auto enter_map(const game_map::gobject_s_ptr_t& object, map_no_t map_no, const vec3& pos, float heading) -> fw::error;
	auto exit_map(const game_map::gobject_s_ptr_t& object) -> fw::error;

private:
	static auto get_nav_dir() -> std::filesystem::path;

private:
	mutable std::shared_mutex lock_{};
	std::unordered_map<map_no_t, std::unique_ptr<game_map>> maps_{};
};
