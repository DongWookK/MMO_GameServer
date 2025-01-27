#pragma once

#include "pch.h"
#include "thread_manager.h"

using namespace fw;

auto thread_pool::setup() -> fw::error
{
	fw::error error_code{};

	error_code = initialize();
	if (error_code != 0)
		return error_code;

	acquire();

	return error_code;
}

auto thread_pool::start() -> fw::error
{
	// fatal :: iocp must set_up

	// 20240127 temp test code
	for (auto& thread : threads_)
	{
		thread->allocate_job();
	}

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
												DWORD aRv = setup_worker();				// pInitfunc 풀의 객체들을 초기화하는 함수
												return aRv;
												}
											 , [](pool_t::TObject* p) {
												 DWORD aRv = teardown_worker();			// pUnAcqFunc
												 return aRv;
												}, thread_count, false);				// size_t pInitSize, bool pIsExpandable

	is_setup_ = true;

	return error_code;
}

auto thread_pool::acquire(uint32_t thread_count) -> void
{
	for (uint32_t i = 0; i < thread_count; ++i)
	{
		thread_pool::object_t thread = pool_.AcquireObject();
		if (nullptr == thread)
		{
			//assert
			break;
		}
		threads_.emplace_back(thread);
	}

	return;
}

auto fw::thread_pool::setup_worker() -> fw::error
{
	// todo :: initialize thread
	return fw::error();
}

auto fw::thread_pool::teardown_worker() -> fw::error
{
	return fw::error();
}


auto thread_manager::setup() -> fw::error
{
	fw::error error_code{};

	error_code = thread_pool_.setup();

	return error_code;
}

auto thread_manager::start() -> fw::error
{
	//notify_all ??  아토믹 변수 기다리기
	is_on_service_.store(true);

	thread_pool_.start();

	return fw::error{};
}

auto thread_manager::stop() -> fw::error
{
	return fw::error();
}

auto thread_manager::teardown() -> fw::error
{
	return fw::error();
}