#pragma once
#include "pch.h"
#include "user_manager.h"

auto user_manager::setup() -> fw::error
{
	return fw::error{};
}

auto user_manager::start() -> fw::error
{
	return fw::error{};
}

auto user_manager::stop() -> fw::error
{
	return fw::error{};
}

auto user_manager::teardown() -> fw::error
{
	return fw::error{};
}

auto user_manager::allocate_user(session_s_ptr_t session) -> void
{
}
