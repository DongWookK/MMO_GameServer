#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::Open()
{

	//�⺻������ ���� �׽�Ʈ
	for (int i = 0; i < _mThreadNo; i++)
	{
		CThreadBase* aNew = new CThreadBase;
		_mThreads.push_back(aNew->Open());
	}

	// threadwk ����


	return 0;
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
