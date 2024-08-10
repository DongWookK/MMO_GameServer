#include "pch_fw.h"
#include "thread_local.h"


thread_local uint32_t LThreadId = 0;
thread_local std::stack<int32_t> LLockStack;
thread_local SendBufferChunkRef	LSendBufferChunk;