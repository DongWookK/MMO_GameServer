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
	error_code = pool_.AllocateChunk<worker>(
		[this]() { return std::make_unique<worker>(); },		// 1. Create
		[](worker* p, size_t i) { p->set_index(i); return 0; }, // 2. Init
		[](worker* p) { /* UnAcquire 처리 */ },					// 3. UnAcquire 람다
		thread_count_,											// 4. pInitSize
		false													// 5. pIsExpandable
	);

	return error_code;
}

auto thread_pool::acquire() -> fw::error
{
	for (uint32_t i = 0; i < thread_count_; ++i)
	{
		thread_pool::object_t worker = pool_.AcquireObject();
		ASSERT_RETURN_VALUE(nullptr != worker, fw::error{});

		threads_.emplace_back(worker);
	}

	return fw::error{};
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

thread_manager::thread_manager(io_context_t* io_context, uint32_t thread_count)
	: io_context_(*io_context)
	, thread_count_(thread_count)
{
}

auto fw::thread_manager::setup() -> fw::error
{
	auto error_code = thread_pool_.setup(thread_count_);
	ASSERT_RETURN_VALUE(!error_code, error_code);

	return error_code;
}

auto thread_manager::start() -> fw::error
{
	is_on_service_.store(true);

	thread_pool_.start(io_context_);

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