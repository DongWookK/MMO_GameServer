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
	// jthread�� stop �� �����ϰ��Ѵ�?
};