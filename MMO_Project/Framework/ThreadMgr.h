#pragma once
#include "ThreadWk.h"
/*---------------------------------------------------------------------
	Thread Manager
desc : Managing Priamry, Worker, Db, Log Thread
		��ô��� �� ������ ������� ����
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

