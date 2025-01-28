#pragma once
#include "pch.h"
#include "thread_manager.h"

int main()
{
	fw::thread_manager::instance()->setup();
	fw::thread_manager::instance()->start();
	fw::thread_manager::instance()->stop();

	std::cout << " end " << std::endl;

	return 0;
}