#pragma once
#include "main.h"
#include "pch.h"
#include "FwCObjectPool.h"
#include <crtdbg.h>

/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

//싱글턴 패턴 적용 필요
enum class Flag {
	OPENED,
	STOPPED,
	ENDED,
	COUNT
};

class thread_pool
{
	using TPool = FnlApi::CObjectPool<std::thread>;
	using TObject = TPool::Object;

public:
	int32_t Initialize();
	int32_t Acquire(uint32 pThreadCount = 1);
	std::vector<TObject>& GetThreads();
	bool	IsOpen();


	static int32_t		OpenWk();
	static int32_t		CloseWk();

private:
	std::shared_ptr<TPool> __mPool;
	std::vector<TObject> __mThreads;
	bool	__mIsOpen = false;
};


class thread_manager : public Singleton<thread_manager>
{
public:
	int32_t		setup();
	int32_t		start();
	int32_t		stop();
	int32_t		teardown();

public:
	Flag		__mFlag = Flag::COUNT;
	thread_pool __mThreadPool;
};


