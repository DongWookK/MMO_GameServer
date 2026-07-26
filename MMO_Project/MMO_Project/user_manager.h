#pragma once
#include "pch.h"
#include <boost/multi_index_container.hpp>
#include "object_pool.hpp"
#include "user.h"

class user_manager : public feature
{
public:
	using pool_t = fw::CObjectPool<user>;
	using object_t = pool_t::Object;

	auto setup() -> fw::error override;
	auto start() -> fw::error override;
	auto stop() -> fw::error override;
	auto teardown() -> fw::error override;

private:
	pool_t user_pool_{};
};