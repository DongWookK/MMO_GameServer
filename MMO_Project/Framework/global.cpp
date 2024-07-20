#include "pch.h"
#include "global.h"
#include "thread_manager.h"
#include "SocketUtils.h"

thread_manager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new thread_manager();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
	}
} GCoreGlobal;