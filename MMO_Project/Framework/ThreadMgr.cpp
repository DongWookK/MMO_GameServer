#include "pch.h"
#include "ThreadMgr.h"


int32 ThreadMgr::open()
{
	int32 aRv = 0;
	aRv = openWk();
	if (aRv != 0)
	{
		CRASH(temp);
	}

	return 0;
}

int32 ThreadMgr::openWk()
{
	int32 aRv = 0;
	//스레드 생성해 unordered map에 저장
	return aRv;
}