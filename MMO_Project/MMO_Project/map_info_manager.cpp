#pragma once
#include "pch.h"
#include "map_info_manager.h"
#include "main_server.h"
#include "map_info_sql.h"

auto map_info_manager::setup() -> fw::error
{
	auto info_sql = main_server::instance()->get_sql<map_info_sql>(common::sql_type::info);
	ASSERT_RETURN_VALUE(info_sql != nullptr, error::code::sql_fail);

	nanodbc::result result;
	auto error = info_sql->map_info_select(result);
	ASSERT_RETURN_VALUE(!error, error);

	while (result.next())
	{
		map_info info{};
		info.map_no = static_cast<map_no_t>(result.get<int32_t>(0));
		const auto type = result.get<uint8_t>(1);
		info.name = result.get<std::wstring>(2);
		info.filename = result.is_null(3) ? std::wstring{} : result.get<std::wstring>(3);

		if (type > fw::to_underlying(common::map_type::MAX))
		{
			FLOG_CRITICAL("map_info_manager :: invalid map type map({}) type({})", info.map_no, type);
			return error::code::map_load_fail;
		}
		info.type = static_cast<common::map_type>(type);

		if (info.name.empty())
		{
			FLOG_CRITICAL("map_info_manager :: empty map name map({})", info.map_no);
			return error::code::map_load_fail;
		}

		if (info.filename.empty())
		{
			FLOG_CRITICAL("map_info_manager :: empty filename map({}:{})", info.map_no, fw::wstring_to_string(info.name));
			return error::code::map_load_fail;
		}

		map_infos_.emplace(info.map_no, std::move(info));
	}

	if (map_infos_.empty())
	{
		FLOG_CRITICAL("map_info_manager :: no map in dt_map");
		return error::code::map_load_fail;
	}

	FLOG_INFO("map_info_manager :: {} map info(s) loaded", map_infos_.size());
	return fw::error{};
}

auto map_info_manager::start() -> fw::error
{
	return fw::error{};
}

auto map_info_manager::stop() -> fw::error
{
	return fw::error{};
}

auto map_info_manager::teardown() -> fw::error
{
	return fw::error{};
}

auto map_info_manager::find_map_info(map_no_t map_no) const -> const map_info*
{
	auto it = map_infos_.find(map_no);
	return it != map_infos_.end() ? &it->second : nullptr;
}
