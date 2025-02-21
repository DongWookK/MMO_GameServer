#pragma once
#include "pch.h"
#include "main_server.h"
#include "thread_manager.h"
#include <boost/asio.hpp>

using namespace boost;

int main()
{
	main_server::instance()->start_service();
	main_server::instance()->stop_service();

	std::cout << " end " << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	return 0;
}