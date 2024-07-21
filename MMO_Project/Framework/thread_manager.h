#pragma once
#include "pch.h"
#include "FwCObjectPool.h"

/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Log_Worker
warn :
----------------------------------------------------------------------*/

uint32_t eThreadCount = 4;

//싱글턴 패턴 적용 필요
enum class Flag {
	NONE,
	START,
	STOP,
	TEARDOWN,
};

class thread_pool
{
	friend thread_manager;

private:
	using TPool = FnlApi::CObjectPool<std::thread>;
	using TObject = TPool::Object;

private:
	auto initialize() -> fw::error;
	auto acquire(uint32_t pThreadCount = 1) -> fw::error;

public:
	static auto open_worker() -> fw::error;
	static auto close_worker() -> fw::error;

public:
	auto is_setup() const -> const bool;
	std::vector<TObject>& get_all_threads();

private:
	std::shared_ptr<TPool> pool_;
	std::vector<TObject> threads_;
	bool is_setup_{};
};


class thread_manager : public singleton<thread_manager>
{
public:
	auto setup() -> fw::error;
	auto start() -> fw::error;
	auto stop() -> fw::error;
	auto teardown() -> fw::error;

public:
	Flag flag_{};
	thread_pool thread_pool_{};
};


