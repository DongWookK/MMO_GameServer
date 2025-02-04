#pragma once
#include "pch.h"

class feature
{
public:
	feature() = delete;
	feature(std::string_view key);
public:
	virtual auto setup() -> fw::error = 0;
	virtual auto start() -> fw::error = 0;
	virtual auto stop() -> fw::error = 0;
	virtual auto teardown() -> fw::error = 0;
public:
	std::string key_{};
};