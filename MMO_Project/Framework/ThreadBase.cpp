#include "pch.h"
#include "ThreadBase.h"

ThreadBase::ThreadBase()
{

}

uint32 ThreadBase::Open()
{
	//Thread Manager에서 이후 threadID 발급
	uintptr_t aThdPtr = _beginthreadex(NULL, 0, &ThreadBase::Start, this, 0, 0);
	return 0;
}

unsigned __stdcall ThreadBase::Start(void *pThis)
{
	ThreadBase* thd = static_cast<ThreadBase*>(pThis);
	thd->Main();

	return 0;
}

void ThreadBase::Main()
{
	while(1)
	{
		this_thread::sleep_for(chrono::seconds(1));
		cout << this_thread::get_id << endl;
	}

	//어떻게 반복에서 나갈까?
}


uint32 ThreadBase::Close()
{
	return 0;
}