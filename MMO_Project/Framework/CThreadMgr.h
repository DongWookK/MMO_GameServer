#pragma once
#include "FwCObjectPool.h"
#include "CThreadBase.h"
/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

//싱글턴 패턴 적용 필요
enum class Flag {
	START,
	INITIAL,
	STOPPED,
	END
};

class CThreadPool
{
	using TPool = CObjectPool<CThreadBase>;
	using Object = TPool::Object;

	uint32_t Initialize();
	Object Acquire();

private:
	std::shared_ptr<TPool> __mPool;
};


class CThreadMgr : public Singleton<CThreadMgr>
{
	using TPool = CObjectPool<CThreadBase>;
public:
	
	Atomic<Flag> _mFlag = Flag::START;

	int32				Open();
	int32				Close();


	int32				OpenWk();

private:
	CThreadPool __mThreadPool;
};

