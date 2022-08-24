#pragma once
#include "pch.h"
#include "CThreadBase.h"
#include "CThreadMgr.h"


void CThreadBase::ResetAttr()
{
	_mThreadMgr = nullptr;
}
DWORD CThreadBase::Open()
{
	//스레드 생성
	uintptr_t aRv = _beginthreadex(NULL, 0, &CThreadBase::Start, this, 0, 0);

	if (aRv == 0)
	{

	}
	else
	{
		_mThdHandler = (DWORD)aRv;
	}
	return _mThdHandler;
}

unsigned __stdcall CThreadBase::Start(void *pThis)
{
	CThreadBase* thd = static_cast<CThreadBase*>(pThis);
	thd->_mThreadMgr = CThreadMgr::This();
	thd->Main();

	return 0;
}

void CThreadBase::Main()
{
	ASSERT_CRASH(_mThreadMgr != nullptr);

	while(Flag::STOPPED != _mThreadMgr->__mFlag)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << std::this_thread::get_id << std::endl;
	}

}



DWORD CThreadBase::Close()
{
	return 0;
}