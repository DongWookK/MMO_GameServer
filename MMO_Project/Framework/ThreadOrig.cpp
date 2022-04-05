#include "pch.h"
#include "ThreadOrig.h"


uint32 ThreadOrig::Open()
{
	//Thread Manager���� ���� threadID �߱�
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

	//��� �ݺ����� ������?
}


uint32 ThreadOrig::Close()
{
	return 0;
}