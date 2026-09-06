#pragma once
#include "pch.h"
#include "session.h"

class user
{
public:
	using session_s_ptr_t = std::shared_ptr<session>;
	using user_no_t = int32_t;
public:
	auto set_index(size_t i) -> void;
	auto set_session(session_s_ptr_t session) -> void;

	auto get_index() const -> const size_t;
	auto get_user_no() const -> const user_no_t;

private:
	size_t index_{};
	session_s_ptr_t session_{};
	user_no_t user_no_{};
};