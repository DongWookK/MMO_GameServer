#pragma once
#include "pch.h"
#include "session.h"

class pc;

class user
{
public:
	using session_s_ptr_t = std::shared_ptr<session>;
	using pc_s_ptr_t = std::shared_ptr<pc>;
	using user_no_t = int32_t;
public:
	auto set_index(size_t i) -> void;
	auto set_session(session_s_ptr_t session) -> void;
	auto set_user_no(user_no_t user_no) -> void;
	auto set_pc(pc_s_ptr_t pc) -> void;
	auto reset() -> void;

	auto get_index() const -> const size_t;
	auto get_session_index() const -> const size_t;
	auto get_user_no() const -> const user_no_t;
	auto get_pc() const -> const pc_s_ptr_t&;

private:
	size_t index_{};
	session_s_ptr_t session_{};
	user_no_t user_no_{};
	pc_s_ptr_t pc_{};
};