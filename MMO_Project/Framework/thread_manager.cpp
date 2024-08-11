#pragma once
#include "pch_fw.h"
#include "thread_manager.h"

/*---------------------------------------------------------------------
	Thread Manager
desc : based on FwCObjectPool
warn :
----------------------------------------------------------------------*/

int32_t thread_pool::initialize()
{
	uint32_t aRv = 0;
	pool_ = std::make_shared<TPool>();
	aRv = pool_->AllocateChunk<worker>([](TPool::TObject* p, size_t i) {
		DWORD aRv = open_worker();					// pInitfunc 풀의 객체들을 초기화하는 함수
		return aRv;
		}, [](TPool::TObject* p) {
			DWORD aRv = close_worker();				// pUnAcqFunc
			return aRv;
		}, eThreadCount, false);					// size_t pInitSize, bool pIsExpandable

	is_setup_ = true;
	return aRv;
}

int32_t thread_pool::acquire(uint32_t pThreadCount)
{
	int32_t aRv = 0;
	for (uint32_t i = 0; i < pThreadCount; ++i)
	{
		thread_pool::TObject aThread = pool_->AcquireObject();
		if (nullptr == aThread)
		{
			aRv = 4;
			break;
		}
		threads_.emplace_back(aThread);
	}

	return aRv;
}

std::vector<thread_pool::TObject>& thread_pool::get_all_threads()
{
	return threads_;
}

auto thread_pool::is_setup() const -> const bool
{
	return is_setup_;
}

auto thread_pool::open_worker() -> fw::error
{
	fw::error error{};

	return error;
}

auto thread_pool::close_worker() -> fw::error
{
	fw::error error{};
	return error;
}

/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

auto thread_manager::setup() -> fw::error
{
	fw::error error = 0;

	error = thread_pool_.initialize();
	if (0 < error)
	{
		return error;
	}

	error = thread_pool_.acquire();
	if (0 < error)
	{
		return error;
	}

	return error;
}

auto thread_manager::start() -> fw::error
{
	ASSERT_CRASH(thread_pool_.is_setup() == true);
	
	fw::error error = 0;
	
	flag_ = Flag::START;

	for (auto& thread : thread_pool_.get_all_threads())
	{
		thread->allocate_job();
	}

	return error;
}

auto thread_manager::stop() -> fw::error
{
	flag_ = Flag::STOP;
	return fw::error{};
}
