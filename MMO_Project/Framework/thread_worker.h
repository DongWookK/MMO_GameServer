#pragma once
#include "pch.h"

class worker
{
public:
	worker();

public:
	auto allocate_job() -> void;
	auto join() -> void;

private:
	static auto job() -> void;

private:
	std::thread thread_;
	// jthread로 stop 을 지원하게한다?
};