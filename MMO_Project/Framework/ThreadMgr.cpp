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
	//������ ������ unordered map�� ����
	return aRv;
}