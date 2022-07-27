#include "pch.h"
#include "CThreadMgr.h"

uint32_t CThreadPool::Initialize()
{
	uint32_t aRv = 0;
	__mPool = std::make_shared<TPool>();
	aRv = __mPool->AllocateChunk<CThreadBase>([](TPool::TObject* p, size_t i) {
		DWORD aRv = p->Open();
		return aRv;
		}, [](TPool::TObject* p) {
			DWORD aRv = p->Close();
			return aRv;
		}
		);
	
	if (0 < aRv)
	{
		//에러로그
	}
	return aRv;
}

CThreadPool::Object CThreadPool::Acquire()
{
	//IsOpen 체크필요
	//하위호출에서 nullptr 체크필요
	return __mPool->AcquireObject();
}


int32 CThreadMgr::Open()
{
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

