
#include "CThreadMgr.h"
#include "pch.h"

int32 CThreadMgr::open()
{
	InitTLS();
	//1)스레드풀 생성
	//2)스레드포인터 배열 하나씩 loop돌면서 launch
	return int32();
}

int32 CThreadMgr::openWk()
{
	uint32 aRv;
	//DB에서 스레드갯수 가져오기 (현재 임시로 5개)
	for (int i = 0; i < WkThreadNo; i++)
	{
		//스레드풀에서 acquire   (임시로 New)
		
	}
	
}

int32 CThreadMgr::InitTLS()
{
	SThreadId = 0;
}

