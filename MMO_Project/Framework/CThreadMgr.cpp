#pragma once
#include "pch.h"
#include "CThreadMgr.h"

int32_t CThreadPool::Initialize()
{
	uint32_t aRv = 0;
	__mPool = std::make_shared<TPool>();
	aRv = __mPool->AllocateChunk<CThreadBase>([](TPool::TObject* p, size_t i) {
		DWORD aRv = p->Open();
		return aRv;
		}, [](TPool::TObject* p) {
			DWORD aRv = p->Close();
			return aRv;
		});

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
			//에러로그
			aRv = 4;
			break;
		}
		__mThreads.emplace_back(aThread);
	}

	return aRv;
}

bool CThreadPool::IsOpen()
{
	return __mIsOpen;
}

int32 CThreadMgr::Open()
{
	int32 aRv = 0;
	aRv = __mThreadPool.Initialize();
	if (0 < aRv)
	{
		//에러로그
		return aRv;
	}

	uint32 aThreadCount = 8;
	aRv = __mThreadPool.Acquire(aThreadCount);

	return aRv;
}

int32 CThreadMgr::Close()
{
	//flag 변경해 loop중인 thread들이 중단하게끔. ( 아직 Pool기반이아닌 vector 리스트이므로 나중엔 freelist로 돌아가게끔?)
	//__mFlag = Flag::STOPPED;
	return 0;
}

int32 CThreadMgr::OpenWk()
{
	uint32 aRv = 0;
	return 0;
	//DB에서 스레드갯수 가져오기 (현재 임시로 5개)
	
	
}

