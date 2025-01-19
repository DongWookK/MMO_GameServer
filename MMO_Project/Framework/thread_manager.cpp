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
												DWORD aRv = setup_worker();					// pInitfunc 풀의 객체들을 초기화하는 함수
												return aRv;
												}
											 , [](pool_t::TObject* p) {
												 DWORD aRv = teardown_worker();				// pUnAcqFunc
												 return aRv;
												}, G_thread_count, false);				// size_t pInitSize, bool pIsExpandable

											 is_setup_ = true;
											 return error_code;
}

auto thread_pool::acquire(uint32_t pThreadCount) -> fw::error
{
	pool_.AcquireObject();

	return fw::error{};
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
