#include "pch.h"
#include "FwTLS.h"


thread_local uint32 LThreadId = 0;
thread_local std::stack<int32>	LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;