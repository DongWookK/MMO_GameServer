#pragma once
#include "pch.h"
#include "user_manager.h"
#include "user_sql.h"
#include "thread_local.h"
#include <cwctype>

auto user_manager::setup() -> fw::error
{
	fw::error error_code{};
	error_code = user_pool_.AllocateChunk<user>(
		[this]() { return std::make_unique<user>(); },			// 1. Create
		[](user* p, size_t i) { p->set_index(i); return 0; },	// 2. Init
		[](user* p) { p->reset(); },							// 3. UnAcquire 람다
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

auto user_manager::user_login(session_s_ptr_t session, std::wstring_view user_name) -> fw::expected<user_s_ptr_t>
{
	auto error = fw::error{};

	ASSERT_RETURN_VALUE(session != nullptr, fw::unexpected(error::code::session_invalid));

	if (!is_valid_user_name(user_name))
	{
		FLOG_WARN("user_login :: invalid user_name length({}) session({})", user_name.size(), session->get_index());
		return fw::unexpected(error::code::user_name_invalid);
	}

	if (find_user(session) != nullptr)
	{
		FLOG_WARN("user_login :: already login session({})", session->get_index());
		return fw::unexpected(error::code::user_already_login);
	}

	auto sql = fw::get_sql<user_sql>();
	ASSERT_RETURN_VALUE(sql != nullptr, fw::unexpected(error::code::sql_stmt_invalid));

	user_no_t user_no{};
	error = sql->user_login(user_name, user_no);
	ASSERT_RETURN_VALUE(!error, fw::unexpected(error));
	ASSERT_RETURN_VALUE(user_no != user_no_t{}, fw::unexpected(error::code::user_login_fail));

	auto user = user_pool_.AcquireObject();
	ASSERT_RETURN_VALUE(nullptr != user, fw::unexpected(error::code::object_acquire_fail));

	user->set_session(session);
	user->set_user_no(user_no);

	auto [it, inserted] = user_list_.insert(user);
	ASSERT_RETURN_VALUE(inserted, fw::unexpected(error::code::user_already_login));

	return *it;
}

auto user_manager::user_logout(session_s_ptr_t session) -> fw::error
{
	ASSERT_RETURN_VALUE(session != nullptr, error::code::session_invalid);

	auto& index = user_list_.get<tag_session>();
	auto it = index.find(session->get_index());
	if (it == index.end())
	{
		FLOG_WARN("user_logout :: not login session({})", session->get_index());
		return error::code::user_not_exist;
	}

	index.erase(it);

	return fw::error{};
}

auto user_manager::find_user(session_s_ptr_t session) const -> user_s_ptr_t
{
	if (session == nullptr)
	{
		return nullptr;
	}

	auto& index = user_list_.get<tag_session>();
	auto it = index.find(session->get_index());
	if (it == index.end())
	{
		return nullptr;
	}

	return *it;
}

auto user_manager::is_valid_user_name(std::wstring_view user_name) -> bool
{
	if (user_name.empty() || user_name.size() > max_user_name_len)
	{
		return false;
	}

	for (const auto ch : user_name)
	{
		if (std::iswcntrl(ch))
		{
			return false;
		}
	}

	return true;
}
