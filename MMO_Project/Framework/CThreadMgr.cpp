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

	//Thread � ���� ���ڹ޵��� ���� �ʿ�
	int32_t aRv = 0;
	for (uint32 i = 0; i < pThreadCount; ++i)
	{
		CThreadPool::TObject aThread = __mPool->AcquireObject();
		if (nullptr == aThread)
		{
			//�����α�
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
		//�����α�
		return aRv;
	}

	uint32 aThreadCount = 8;
	aRv = __mThreadPool.Acquire(aThreadCount);

	return aRv;
}

int32 CThreadMgr::Close()
{
	//flag ������ loop���� thread���� �ߴ��ϰԲ�. ( ���� Pool����̾ƴ� vector ����Ʈ�̹Ƿ� ���߿� freelist�� ���ư��Բ�?)
	//__mFlag = Flag::STOPPED;
	return 0;
}

int32 CThreadMgr::OpenWk()
{
	uint32 aRv = 0;
	return 0;
	//DB���� �����尹�� �������� (���� �ӽ÷� 5��)
	
	
}

