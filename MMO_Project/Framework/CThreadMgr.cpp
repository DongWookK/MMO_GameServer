#include "pch.h"
#include "CThreadMgr.h"

uint32_t CThreadPool::Initialize()
{
	uint32_t aRv = 0;
	__mPool = std::make_shared<TPool>();
	aRv = __mPool->AllocateChunk<CThreadBase>([](TPool::TObject* p, size_t i) {
		DWORD aRv = p->Open();
		return aRv;
		}, [](TPool::TObject* p) {
			DWORD aRv = p->Close();
			return aRv;
		}
		);
	
	if (0 < aRv)
	{
		//�����α�
	}
	return aRv;
}

CThreadPool::Object CThreadPool::Acquire()
{
	//IsOpen üũ�ʿ�
	//����ȣ�⿡�� nullptr üũ�ʿ�
	return __mPool->AcquireObject();
}


int32 CThreadMgr::Open()
{
}

int32 CThreadMgr::Close()
{
	//flag ������ loop���� thread���� �ߴ��ϰԲ�. ( ���� Pool����̾ƴ� vector ����Ʈ�̹Ƿ� ���߿� freelist�� ���ư��Բ�?)
	_mFlag = Flag::STOPPED;
	return 0;
}

int32 CThreadMgr::OpenWk()
{
	uint32 aRv;
	return 0;
	//DB���� �����尹�� �������� (���� �ӽ÷� 5��)
	
	
}

