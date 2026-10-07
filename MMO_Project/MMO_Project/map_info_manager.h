#pragma once
#include "pch.h"
#include "gobject.h"

struct map_info
{
	using map_no_t = gobject::map_no_t;

	map_no_t map_no{};
	common::map_type type{ common::map_type::permanent };
	std::wstring name{};
	std::wstring filename{};
};

class map_info_manager : public feature, public singleton<map_info_manager>
{
	friend class singleton<map_info_manager>;

public:
	using map_no_t = map_info::map_no_t;
	using map_info_list_t = std::unordered_map<map_no_t, map_info>;

public:
	map_info_manager()
		: feature("map_info_manager")
	{}
	~map_info_manager() override = default;

public:
	auto setup() -> fw::error override;
	auto start() -> fw::error override;
	auto stop() -> fw::error override;
	auto teardown() -> fw::error override;

public:
	auto find_map_info(map_no_t map_no) const -> const map_info*;
	auto get_map_infos() const -> const map_info_list_t& { return map_infos_; }

private:
	map_info_list_t map_infos_{};
};
