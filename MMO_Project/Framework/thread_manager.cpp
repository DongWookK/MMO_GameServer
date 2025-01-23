#pragma once

#include "pch.h"
#include "thread_manager.h"

using namespace fw;

auto thread_pool::setup() -> fw::error
{
	fw::error error_code{};

	error_code = acquire();

	return error_code;
}

auto thread_pool::start() -> fw::error
{
	return fw::error();
}

auto thread_pool::stop() -> fw::error
{
	return fw::error();
}

auto thread_pool::teardown() -> fw::error
{
	return fw::error();
}

auto thread_pool::initialize() -> fw::error
{
	fw::error error_code{};
	error_code = pool_.AllocateChunk<worker>([](pool_t::TObject* p, size_t i) {
												DWORD aRv = setup_worker();				// pInitfunc Ǯ�� ��ü���� �ʱ�ȭ�ϴ� �Լ�
												return aRv;
												}
											 , [](pool_t::TObject* p) {
												 DWORD aRv = teardown_worker();			// pUnAcqFunc
												 return aRv;
												}, G_thread_count, false);				// size_t pInitSize, bool pIsExpandable

											 is_setup_ = true;
											 return error_code;
}

auto thread_pool::acquire(uint32_t thread_count) -> fw::error
{
	fw::error error_code{};

	for (uint32_t i = 0; i < thread_count; ++i)
	{
		thread_pool::object_t thread = pool_.AcquireObject();
		if (nullptr == thread)
		{
			error_code = 1;
			break;
		}
		threads_.emplace_back(thread);
	}

	return error_code;
}

auto fw::thread_pool::setup_worker() -> fw::error
{
	
	return fw::error();
}

auto fw::thread_pool::teardown_worker() -> fw::error
{
	return fw::error();
}


auto thread_manager::setup() -> fw::error
{
	return fw::error();
}

auto thread_manager::start() -> fw::error
{
	return fw::error();
}

auto thread_manager::stop() -> fw::error
{
	return fw::error();
}

auto thread_manager::teardown() -> fw::error
{
	return fw::error();
}