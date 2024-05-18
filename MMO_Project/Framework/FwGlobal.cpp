#include "pch.h"
#include "FwGlobal.h"
#include "CThreadMgr.h"
#include "SocketUtils.h"

CThreadMgr* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new CThreadMgr();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;