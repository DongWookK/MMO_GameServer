#pragma once
#include "pch.h"
#include "user.h"
#include "pc.h"

auto user::set_index(size_t index) -> void
{
	index_ = index;
}

auto user::set_session(session_s_ptr_t session) -> void
{
	session_ = session;
}

auto user::set_user_no(user_no_t user_no) -> void
{
	user_no_ = user_no;
}

auto user::set_pc(pc_s_ptr_t pc) -> void
{
	pc_ = std::move(pc);
}

auto user::reset() -> void
{
	session_.reset();
	user_no_ = {};
	pc_.reset();
}

auto user::get_index() const -> const size_t
{
	return index_;
}

auto user::get_session_index() const -> const size_t
{
	return session_ ? session_->get_index() : (std::numeric_limits<size_t>::max)();
}

auto user::get_user_no() const -> const user_no_t
{
	return user_no_;
}

auto user::get_pc() const -> const pc_s_ptr_t&
{
	return pc_;
}
