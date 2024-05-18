#pragma once
#include "pch.h"
#include "CThreadMgr.h"

/*---------------------------------------------------------------------
	Thread Manager
desc : based on FwCObjectPool
warn :
----------------------------------------------------------------------*/

int32_t CThreadPool::Initialize()
{
	uint32_t aRv = 0;
	__mPool = std::make_shared<TPool>();
	aRv = __mPool->AllocateChunk<std::thread>([](TPool::TObject* p, size_t i) {
		DWORD aRv = OpenWk();					// pInitfunc
		return aRv;
		}, [](TPool::TObject* p) {
			DWORD aRv = CloseWk();				// pUnAcqFunc
			return aRv;
		},eThreadCount,false);					// size_t pInitSize, bool pIsExpandable

	__mIsOpen = true;
	return aRv;
}

int32_t CThreadPool::Acquire(uint32 pThreadCount)
{
	//Thread 몇개 열지 인자받도록 수정 필요
	int32_t aRv = 0;
	for (uint32 i = 0; i < pThreadCount; ++i)
	{
		CThreadPool::TObject aThread = __mPool->AcquireObject();
		if (nullptr == aThread)
		{
			//임시에러로그 4
			aRv = 4;
			break;
		}
		__mThreads.emplace_back(aThread);
	}
	return aRv;
}

std::vector<CThreadPool::TObject>& CThreadPool::GetThreads()
{
	return __mThreads;
}

bool CThreadPool::IsOpen()
{
	return __mIsOpen;
}

int32 CThreadPool::OpenWk()
{
	uint32 aRv = 0;

	while (Flag::OPENED == CThreadMgr::This()->__mFlag)
	{
		// thread work
		std::thread::id this_id = std::this_thread::get_id();

		std::cout << "thread " << this_id << " sleeping...\n";

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return aRv;
}

int32 CThreadPool::CloseWk()
{
	uint32 aRv = 0;
	return aRv;
}

/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
warn :
----------------------------------------------------------------------*/

int32 CThreadMgr::Open()
{
	int32 aRv = 0;
	__mFlag = Flag::OPENED;

	aRv = __mThreadPool.Initialize();
	if (0 < aRv)
	{
		//에러로그
		return aRv;
	}
	aRv = __mThreadPool.Acquire(eThreadCount);

	return aRv;
}

int32 CThreadMgr::Close()
{
	//flag 변경해 loop중인 thread들이 중단하게끔. ( 아직 Pool기반이아닌 vector 리스트이므로 나중엔 freelist로 돌아가게끔?)
	__mFlag = Flag::STOPPED;
	return 0;
}
