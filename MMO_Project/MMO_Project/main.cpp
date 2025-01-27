#pragma once
#include "pch.h"
#include "thread_manager.h"

int main()
{
	fw::thread_manager::instance()->setup();
	fw::thread_manager::instance()->start();

	return 0;
}