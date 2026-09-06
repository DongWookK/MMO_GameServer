#pragma once
#include "pch.h"
#include "user_manager.h"

auto user_manager::setup() -> fw::error
{
	fw::error error_code{};
	error_code = user_pool_.AllocateChunk<user>(
		[this]() { return std::make_unique<user>(); },			// 1. Create
		[](user* p, size_t i) { p->set_index(i); return 0; },	// 2. Init
		[](user* p) { /* UnAcquire 처리 */ },					// 3. UnAcquire 람다
		1000,													// 4. pInitSize
		false													// 5. pIsExpandable
	);

	return error_code;
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

auto user_manager::user_login(session_s_ptr_t session) -> fw::error
{
	return fw::error();
}

auto user_manager::find_user(session_s_ptr_t session) const -> user_s_ptr_t
{
	return user_s_ptr_t();
}

auto user_manager::allocate_user() -> user_s_ptr_t
{
	auto user = user_pool_.AcquireObject();
	ASSERT_RETURN_VALUE(nullptr != user, nullptr);

	return user;
}
