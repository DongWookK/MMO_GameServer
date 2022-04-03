#pragma once
#include "ThreadWk.h"
/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
		언올더드 맵 단위로 스레드들 관리
warn :
----------------------------------------------------------------------*/

class ThreadMgr
{
public:
	int32				open();
	int32				openWk();
private:
	unordered_map<uint32, ThreadWk> WorkerPool;
	
};

