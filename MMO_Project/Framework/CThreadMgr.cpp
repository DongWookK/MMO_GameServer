#include "pch.h"
#include "CThreadMgr.h"
#include "CThreadBase.h"

int32 CThreadMgr::Open()
{
	// threadwk 생성
	// close될 경우 factory로 돌아가게해야하지않을까?
	__mThreadPool = std::make_shared<TPool>();
	__mThreadPool->AllocateChunk<CThreadBase>([](TPool::TObject * p, size_t i) {
		DWORD aRv = p->Open();
		return aRv;
		}
	);
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
