#pragma once

#include "pch.h"
#include "thread_manager.h"

using namespace fw;

auto thread_pool::setup(const uint32_t thread_count) -> fw::error
{
	fw::error error_code{};

	error_code = initialize(thread_count);
	ASSERT_RETURN_VALUE(!(error_code), error_code);

	acquire();

	return error_code;
}

auto thread_pool::start(io_context_t& io_context) -> fw::error
{
	for (auto& thread : threads_)
	{
		thread->allocate_job(io_context);
	}

	return fw::error();
}

auto thread_pool::stop() -> fw::error
{
	/* jthread라서 풀에서 비우기만하면된다?
	for (auto& thread : threads_) {
		thread->join();
	}
	*/

	return fw::error();
}

auto thread_pool::teardown() -> fw::error
{
	return fw::error();
}

auto thread_pool::initialize(const uint32_t thread_count) -> fw::error
{
	thread_count_ = thread_count;

	fw::error error_code{};
	error_code = pool_.AllocateChunk<worker>([](pool_t::TObject* p, size_t i) {
												DWORD aRv = setup_worker();
												return aRv;
												}
											 , [](pool_t::TObject* p) {
												 DWORD aRv = teardown_worker();
												 return aRv;
												}, thread_count_, false);				// size_t pInitSize, bool pIsExpandable

	return error_code;
}

auto thread_pool::acquire() -> fw::error
{
	for (uint32_t i = 0; i < thread_count_; ++i)
	{
		thread_pool::object_t thread = pool_.AcquireObject();
		ASSERT_RETURN_VALUE(nullptr == thread, 111);

		threads_.emplace_back(thread);
	}

	return 0;
}

auto fw::thread_pool::setup_worker() -> fw::error
{
	// todo :: initialize thread
	return fw::error();
}

auto fw::thread_pool::teardown_worker() -> fw::error
{
	// todo :: gracefule shutdown
	return fw::error();
}

auto thread_manager::setup(boost::asio::io_context& ioc, uint32_t thread_count) -> fw::error
{
	fw::error error_code{};

	error_code = thread_pool_.setup(thread_count);
	ASSERT_RETURN_VALUE(!(error_code), error_code);

	return error_code;
}

auto thread_manager::start() -> fw::error
{
	is_on_service_.store(true);

	thread_pool_.start(main_server::instac);

	return fw::error{};
}

auto thread_manager::stop() -> fw::error
{
	thread_pool_.stop();

	return fw::error();
}

auto thread_manager::teardown() -> fw::error
{
	return fw::error();
}