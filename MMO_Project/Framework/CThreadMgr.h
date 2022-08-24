#pragma once
#include "main.h"
#include "FwCObjectPool.h"
#include "pch.h"
#include <crtdbg.h>

/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

//싱글턴 패턴 적용 필요
enum class Flag {
	START,
	OPENED,
	STOPPED,
	END
};

class CThreadPool
{
	using TPool = FnlApi::CObjectPool<CThreadBase>;
	using TObject = TPool::Object;

public:
	int32_t Initialize();
	int32_t Acquire(uint32 pThreadCount = 1);
	
	bool	IsOpen();

private:
	std::shared_ptr<TPool> __mPool;
	std::vector<TObject> __mThreads;
	bool	__mIsOpen = false;
};


class CThreadMgr : public Singleton<CThreadMgr>
{
public:
	
	int32_t				Open();
	int32_t				Close();
	int32_t				OpenWk();

public:
	Flag		__mFlag = Flag::END; //상태플래그

private:
	CThreadPool __mThreadPool;

	
};

