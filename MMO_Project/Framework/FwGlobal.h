#pragma once


extern class CThreadMgr threadMgr;

/*
ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;

DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
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


*/