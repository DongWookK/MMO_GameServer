#pragma once
#include "pch.h"
#include "main_server.h"

int main()
{
	spdlog::info("Welcome to spdlog!");
	spdlog::error("Some error message with arg: {}", 1);

	main_server::instance()->start_service();
	main_server::instance()->stop_service();

	std::string message{};
	bool out = true;
	std::cout << "server :: start_service" << std::endl;
	while (out)
	{
	std::cin >> message;

	if ("stop" == message)
	{
		out = false;
	}
	}

	std::cout << " server :: stop_service " << std::endl;

	return 0;
}
