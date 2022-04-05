#include "pch.h"
#include "ThreadOrig.h"


uint32 ThreadOrig::Open()
{
	//Thread Manager에서 이후 threadID 발급
	uintptr_t aThdPtr = _beginthreadex(NULL, 0, &ThreadOrig::Start, this, 0, 0);
	return 0;
}

unsigned __stdcall ThreadOrig::Start(void *pThis)
{
	ThreadOrig* thd = static_cast<ThreadOrig*>(pThis);
	thd->Main();

	return 0;
}

void ThreadOrig::Main()
{
	while(1)
	{
		this_thread::sleep_for(chrono::seconds(1));
		cout << this_thread::get_id << endl;
	}

	//어떻게 반복에서 나갈까?
}


uint32 ThreadOrig::Close()
{
	return 0;
}