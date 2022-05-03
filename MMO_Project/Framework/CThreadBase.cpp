#include "pch.h"
#include "CThreadBase.h"

CThreadBase::CThreadBase()
{

}

HANDLE CThreadBase::Open()
{
	//스레드 생성
	uintptr_t aRv = _beginthreadex(NULL, 0, &CThreadBase::Start, this, 0, 0);

	if (aRv == 0)
	{
		//추후 에러로그, 크러쉬
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
	//flag가 ~로 바뀌면 stopped
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