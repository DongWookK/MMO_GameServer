#pragma once
#include "pch.h"

class worker
{
public:
	worker();

public:
	auto allocate_job() -> const uint32_t;
	static auto job() -> const uint32_t;

private:
	std::thread thread_; // 단순히 멤버로 thread를 가지고 있는상태에서는 아무기능을 하지 않는다.
	// jthread로 stop 을 지원하게한다?
};