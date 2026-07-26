#pragma once
#include "pch.h"
#include "session.h"

class user
{
public:
	using session_ptr_t = std::shared_ptr<session>;

public:
	user() = delete;
	user(session_ptr_t session_s_ptr);

private:
	session_ptr_t session_{};
};