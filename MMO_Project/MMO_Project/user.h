#pragma once
#include "pch.h"
#include "session.h"

class user
{
public:
	using session_s_ptr_t = std::shared_ptr<session>;

public:
	auto set_index(size_t i) -> void;
	auto set_session(session_s_ptr_t session) -> void;
private:
	size_t index_{};
	session_s_ptr_t session_{};
};