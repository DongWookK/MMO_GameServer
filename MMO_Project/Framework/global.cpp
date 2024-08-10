#pragma once

#include "pch_fw.h"
#include "global.h"
#include "thread_manager.h"

thread_manager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new thread_manager();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;