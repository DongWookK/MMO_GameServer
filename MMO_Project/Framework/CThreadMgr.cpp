#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::open()
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

int32 CThreadMgr::openWk()
{
	uint32 aRv;
	return 0;
	//DB���� �����尹�� �������� (���� �ӽ÷� 5��)
	
	
}
