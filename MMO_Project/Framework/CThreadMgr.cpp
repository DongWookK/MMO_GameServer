#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::open()
{

	//기본스레드 생성 테스트
	for (int i = 0; i < _mThreadNo; i++)
	{
		CThreadBase* aNew = new CThreadBase;
		_mThreads.push_back(aNew->Open());
	}

	// threadwk 생성


	return 0;
}

int32 CThreadMgr::openWk()
{
	uint32 aRv;
	return 0;
	//DB에서 스레드갯수 가져오기 (현재 임시로 5개)
	
	
}
