#pragma once
#include "pch.h"
#include "db_manager.h"

extern thread_local uint32_t thread_Id_;

namespace fw::tls {
	using db_list_t = std::array<db_manager, fw::to_underlying(common::sql_type::MAX) + 1>;

	extern thread_local db_list_t* db_list;
}

namespace fw {
	template<typename T>
	auto get_sql(common::sql_type type) -> T*
	{
		ASSERT_RETURN_VALUE(tls::db_list != nullptr, nullptr);

		const auto index = fw::to_underlying(type);
		ASSERT_RETURN_VALUE(index < tls::db_list->size(), nullptr);

		auto sql = (*tls::db_list)[index].get_sql<T>();
		ASSERT_RETURN_VALUE(sql != nullptr, nullptr);

		return sql;
	}

	template<typename T>
	auto get_sql() -> T*
	{
		ASSERT_RETURN_VALUE(tls::db_list != nullptr, nullptr);

		for (auto& db : *tls::db_list)
		{
			if (auto sql = db.get_sql<T>())
			{
				return sql;
			}
		}

		ASSERT_RETURN_VALUE(false, nullptr);
	}
}
