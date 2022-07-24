#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::Open()
{
	// threadwk 생성
	const auto& aInit = [this]()->DWORD {return this->Open(); };

	return 0;
}

int32 CThreadMgr::Close()
{
	//flag 변경해 loop중인 thread들이 중단하게끔. ( 아직 Pool기반이아닌 vector 리스트이므로 나중엔 freelist로 돌아가게끔?)
	_mFlag = Flag::STOPPED;
	return 0;
}

int32 CThreadMgr::OpenWk()
{
	uint32 aRv;
	return 0;
	//DB에서 스레드갯수 가져오기 (현재 임시로 5개)
	
	
}
