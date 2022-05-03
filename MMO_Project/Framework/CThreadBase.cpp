#include "pch.h"
#include "CThreadBase.h"

CThreadBase::CThreadBase()
{

}

HANDLE CThreadBase::Open()
{
	//������ ����
	uintptr_t aRv = _beginthreadex(NULL, 0, &CThreadBase::Start, this, 0, 0);

	if (aRv == 0)
	{
		//���� �����α�, ũ����
	}
	else
	{
		_mThdHandler = (HANDLE)aRv;
	}
	return _mThdHandler;
}

unsigned __stdcall CThreadBase::Start(void *pThis)
{
	CThreadBase* thd = static_cast<CThreadBase*>(pThis);
	thd->Main();

	return 0;
}

void CThreadBase::Main()
{
	//flag�� ~�� �ٲ�� stopped
	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << std::this_thread::get_id << std::endl;
	}

}



uint32 CThreadBase::Close()
{
	return 0;
}