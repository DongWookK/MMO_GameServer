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

auto user_manager::user_login(session_s_ptr_t session) -> user_s_ptr_t
{
	auto error = fw::error{};

	auto user = user_pool_.AcquireObject();
	ASSERT_RETURN_VALUE(nullptr != user, nullptr);

	user->set_session(session);
	ASSERT_RETURN_VALUE(user_list_.insert(user).second, nullptr);

	auto it = user_list_.insert(user).first;
	ASSERT_RETURN_VALUE(user_list_.end() != it, nullptr);
	
	return *it;
}

auto user_manager::user_logout(session_s_ptr_t session) -> fw::error
{
	auto error = fw::error{};
	
	auto user = find_user(session);
	ASSERT_RETURN_VALUE(user != nullptr, error::code::UserNotExist);

	auto& key_index = user_list_.get<tag_key>();
	auto it = key_index.find(user->get_index());
	if (it != key_index.end())
	{
		key_index.erase(it);
	}

	return error;
}

auto user_manager::find_user(session_s_ptr_t session) const -> user_s_ptr_t
{
	auto& index = user_list_.get<tag_key>();
	auto it = index.find(session->get_index());
	ASSERT_RETURN_VALUE(it != index.end(), nullptr);
	
	return *it;
}