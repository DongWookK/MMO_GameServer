#pragma once
#include "pch.h"
#include "user.h"

auto user::set_index(size_t index) -> void
{
	index_ = index;
}

auto user::set_session(session_s_ptr_t session) -> void
{
	session_ = session;
}
