#include "pch.h"
#include "FwGlobal.h"
#include "CThreadMgr.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"

CThreadMgr* GThreadManager = nullptr;
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new CThreadMgr();
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferManager;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GCoreGlobal;