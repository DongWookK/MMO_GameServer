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

class CThreadPool
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


class CThreadMgr : public Singleton<CThreadMgr>
{
public:
	
	int32_t		Open();
	int32_t		Close();

public:
	Flag		__mFlag = Flag::COUNT;
	CThreadPool __mThreadPool;
};


