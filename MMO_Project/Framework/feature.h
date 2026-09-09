#pragma once
#include "pch.h"

class feature
{
public:
	feature(std::string_view key);

public:
	virtual auto setup() -> fw::error { return fw::error{}; };
	virtual auto start() -> fw::error { return fw::error{}; };
	virtual auto stop() -> fw::error { return fw::error{}; };
	virtual auto teardown() -> fw::error { return fw::error{}; };
public:
	std::string key_{};
};