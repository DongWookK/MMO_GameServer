#include "pch.h"
#include "CThreadBase.h"

CThreadBase::CThreadBase()
{

}

uint32 CThreadBase::Open()
{
	//Thread Manager에서 threadID 발급
	


	uintptr_t aThdPtr = _beginthreadex(NULL, 0, &CThreadBase::Start, this, 0, 0);
	return 0;
}

unsigned __stdcall CThreadBase::Start(void *pThis)
{
	CThreadBase* thd = static_cast<CThreadBase*>(pThis);
	thd->Main();

	return 0;
}

void CThreadBase::Main()
{
	while(1)
	{
		this_thread::sleep_for(chrono::seconds(1));
		cout << this_thread::get_id << endl;
	}

	//어떻게 반복에서 나갈까?
}

uint32 CThreadBase::AllocateThreadId()
{
	LThreadId = SThreadId.fetch_add(1);
	return uint32();
}


uint32 CThreadBase::Close()
{
	return 0;
}